<?hh // strict

/*

	Hashids
	http://hashids.org/php
	(c) 2013 Ivan Akimov
	(c) 2015 Artem Chudinov

	https://github.com/ivanakimov/hashids.php
	hashids may be freely distributed under the MIT license.

*/

namespace Hashids;

class Hashids implements HashGenerator {

	const VERSION = '1.0.5';

	/* internal settings */

	const int MAX_INT_VALUE = PHP_INT_MAX;
	const ALPHABET_DEFAULT = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890';
	const MIN_ALPHABET_LENGTH = 16;
	const SEP_DIV = 3.5;
	const GUARD_DIV = 12;

	/* error messages */

	const E_ALPHABET_SPACE = 'alphabet cannot contain spaces';

	/* set at constructor */

	private array<int> $_alphabet;
	private int $_alphabet_length;
	private array<int> $_seps;
	private int $_seps_length;
	private array<string> $_seps_chars = ['c','f','h','i','s','t','u','C','F','H','I','S','T','U'];
	private int $_min_hash_length = 0;
	private array<int> $_guards;
	private array<int> $_salt;

	public function __construct(string $salt = '', int $min_hash_length = 0, string $alphabet = '') {

		/* handle parameters */

		$this->_seps = array_map(
			fun('ord'),
			$this->_seps_chars
		);
		$this->_salt = array_map(
			fun('ord'),
			str_split($salt)
		);
		$this->_alphabet = array_map(
			fun('ord'),
			str_split($alphabet ?: self::ALPHABET_DEFAULT)
		);


		if (count($this->_alphabet) < self::MIN_ALPHABET_LENGTH) {
			throw new \Exception(sprintf('alphabet must contain at least %d unique characters', self::MIN_ALPHABET_LENGTH));
		}

		if (in_array(20, $this->_alphabet)) /* 20 = ord(' ') */
		{
			throw new \Exception(self::E_ALPHABET_SPACE);
		}

		/* max(0, min_hash_length) is 3 times slower */
		if ($min_hash_length > 0) {
			$this->_min_hash_length = $min_hash_length;
		}

		$this->_seps = array_intersect($this->_alphabet, $this->_seps);
		$this->_seps_length = count($this->_seps);
		$this->_alphabet = array_diff($this->_alphabet, $this->_seps);
		$this->_seps = $this->_consistent_shuffle($this->_seps, $this->_seps_length, $this->_salt);

		if (!$this->_seps || (count($this->_alphabet) / $this->_seps_length) > self::SEP_DIV) {

			$seps_length = (int)ceil(count($this->_alphabet) / self::SEP_DIV);

			if ($seps_length === 1) {
				$seps_length++;
			}

			if ($seps_length > $this->_seps_length) {

				$diff = $seps_length - $this->_seps_length;
				$this->_seps = array_merge(
					$this->_seps,
					array_slice($this->_alphabet, 0, $diff)
				);
				$this->_alphabet = array_slice($this->_alphabet, $diff);

			} else {
				$this->_seps = array_slice($this->_seps, 0, $seps_length);
			}

		}

		$this->_alphabet_length = count($this->_alphabet);
		$this->_alphabet = $this->_consistent_shuffle($this->_alphabet, $this->_alphabet_length, $this->_salt);
		$guard_count = (int)ceil($this->_alphabet_length / self::GUARD_DIV);

		if (count($this->_alphabet) < 3) {
			$this->_guards = array_slice($this->_seps, 0, $guard_count);
			$this->_seps = array_slice($this->_seps, $guard_count);
		} else {
			$this->_guards = array_slice($this->_alphabet, 0, $guard_count);
			$this->_alphabet = array_slice($this->_alphabet, $guard_count);
			$this->_alphabet_length = count($this->_alphabet);
		}
		$this->_seps_length = count($this->_seps);
	}

	public function encode(array<int> $numbers): string {

		$ret = '';
		if (!count($numbers)) {
			return $ret;
		}

		foreach ($numbers as $number) {

			if ($number < 0) {
				return $ret;
			}

		}

		return implode('', array_map(fun('chr'), $this->_encode($numbers)));

	}

	public function decode(string $hash): array<int> {

		$ret = [];

		
		// ...Maybe it is faster when `trim` is in `if`
		if (!($hash = trim($hash))) {
			return $ret;
		}

		return $this->_decode($hash, $this->_alphabet);

	}

	public function encode_hex(string $str): string {

		if (!ctype_xdigit($str)) {
			return '';
		}

		$numbers = trim(chunk_split($str, 12, ' '));
		$numbers = explode(' ', $numbers);

		foreach ($numbers as $i => $number) {
			$numbers[$i] = hexdec('1' . $number);
		}

		return $this->encode($numbers);

	}

	public function decode_hex(string $hash): string {

		$ret = '';
		$numbers = $this->decode($hash);

		foreach ($numbers as $i => $number) {
			$ret .= substr(dechex($number), 1);
		}

		return $ret;

	}

	public function get_max_int_value(): int {
		return self::MAX_INT_VALUE;
	}

	private function _encode(array<int> $numbers): array<int> {

		$alphabet = $this->_alphabet;
		$numbers_size = count($numbers);
		$numbers_hash_int = 0;

		foreach ($numbers as $i => $number) {
			$numbers_hash_int += ($number % ($i + 100));
		}

		$lottery = $ret = [
			$alphabet[$numbers_hash_int % $this->_alphabet_length]
		];
		foreach ($numbers as $i => $number) {

			$alphabet = $this->_consistent_shuffle(
				$alphabet,
				$this->_alphabet_length,
				array_slice(
					array_merge(
						$lottery,
						$this->_salt,
						$alphabet,
					),
					0,
					$this->_alphabet_length
				)
			);
			$last = $this->_hash($number, $alphabet);
			$ret = array_merge($ret, $last);

			if ($i + 1 < $numbers_size) {
				$number %= ($last[0] + $i);
				$seps_index = $number % $this->_seps_length;
				$ret[] = $this->_seps[$seps_index];
			}

		}

		if (count($ret) < $this->_min_hash_length) {

			$guard_index = ($numbers_hash_int + $ret[0]) % count($this->_guards);

			$guard = $this->_guards[$guard_index];
			array_unshift($ret, $guard);

			if (count($ret) < $this->_min_hash_length) {

				$guard_index = ($numbers_hash_int + $ret[2]) % count($this->_guards);
				$guard = $this->_guards[$guard_index];

				array_unshift($ret, $guard);

			}

		}

		$half_length = (int)($this->_alphabet_length / 2);
		while (count($ret) < $this->_min_hash_length) {

			$alphabet = $this->_consistent_shuffle($alphabet, $this->_alphabet_length, $alphabet);
			$ret = array_merge(
				array_slice($alphabet, $half_length),
				$ret,
				array_slice($alphabet, 0, $half_length)
			);

			$excess = count($ret) - $this->_min_hash_length;
			if ($excess > 0) {
				$ret = array_slice($ret, $excess / 2, $this->_min_hash_length);
			}

		}

		return $ret;

	}

	private function _decode(string $hash, array<int> $alphabet): array<int> {

		$ret = [];

		$hash_breakdown = str_replace(
			array_map(fun('chr'), $this->_guards),
			' ',
			$hash
		);
		$hash_array = explode(' ', $hash_breakdown);

		$i = 0;
		if (count($hash_array) === 3 || count($hash_array) === 2) {
			$i = 1;
		}

		$hash_breakdown = $hash_array[$i];
		if (strlen($hash_breakdown)) {

			$lottery = ord($hash_breakdown[0]);
			$hash_breakdown = substr($hash_breakdown, 1);

			$hash_breakdown = str_replace(
				$this->_seps_chars, ' ', $hash_breakdown
			);
			$hash_array = explode(' ', $hash_breakdown);

			foreach ($hash_array as $sub_hash) {
				$alphabet = $this->_consistent_shuffle(
					$alphabet,
					$this->_alphabet_length,
					array_slice(
						array_merge([$lottery], $this->_salt, $alphabet),
						0,
						$this->_alphabet_length
					)
				);
				$ret[] = $this->_unhash($sub_hash, $alphabet);
			}

			if (implode('', array_map(fun('chr'), $this->_encode($ret)))
				!== $hash)
			{
				$ret = [];
			}

		}

		return $ret;

	}

	private function _consistent_shuffle(array<int> $alphabet, int $alphabet_len, array<int> $salt): array<int> {

		$salt_len = count($salt);
		if (!$salt_len) {
			return $alphabet;
		}

		$alphabet = array_values($alphabet);

		for ($i = $alphabet_len - 1, $v = 0, $p = 0; $i > 0; $i--, $v++) {

			$v %= $salt_len;
			$p += $int = $salt[$v];
			$j = ($int + $v + $p) % $i;

			$temp = $alphabet[$j];
			$alphabet[$j] = $alphabet[$i];
			$alphabet[$i] = $temp;

		}

		return $alphabet;

	}

	private function _hash(int $input, array<int> $alphabet): array<int> {

		$hash = [];

		do {

			array_unshift($hash, $alphabet[$input % $this->_alphabet_length]);

			$input = gmp_intval(gmp_div($input, $this->_alphabet_length));
			//$input = (int)($input / $this->_alphabet_length);

		} while ($input !== 0);

		return $hash;

	}

	private function _unhash(string $input, array<int> $alphabet): int {

		$number = gmp_init(0);
		$input_len = strlen($input);
		if ($input_len && $alphabet) {

			$alphabet_length = count($alphabet);
			$input_chars = str_split($input);

			foreach ($input_chars as $i => $char) {

				$pos = array_search(ord($char), $alphabet);
				$number = gmp_add(
					$number,
					$pos * pow($alphabet_length, ($input_len - $i - 1))
				);
				//$number += $pos * pow($alphabet_length, (strlen($input) - $i - 1));

			}

		}

		return gmp_intval($number);
	}
}
