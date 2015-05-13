<?hh // strict

/*

	Hashids
	http://hashids.org/php
	(c) 2013 Ivan Akimov

	https://github.com/ivanakimov/hashids.php
	hashids may be freely distributed under the MIT license.

*/

namespace Hashids;

/**
 * HashGenerator is a contract for generating hashes
 */
interface HashGenerator {

	/**
	 * Encodes a variable number of parameters to generate a hash
	 * 
	 * @param array<int> array of non-negative numbers
	 * 
	 * @return string the generated hash
	 */
	public function encode(array<int> $numbers): string;

	/**
	 * Decodes a hash to the original parameter values
	 * 
	 * @param string $hash the hash to decode
	 * 
	 * @return array<int>
	 */
	public function decode(string $hash): array<int>;

	/**
	 * Encodes hexadecimal values to generate a hash
	 * 
	 * @param string $str hexadecimal string
	 * 
	 * @return string the generated hash
	 */
	public function encode_hex(string $str): string;

	/**
	 * Decodes hexadecimal hash
	 * 
	 * @param string $hash
	 * 
	 * @return string hexadecimal string
	 */
	public function decode_hex(string $hash): string;

}
