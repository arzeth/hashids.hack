<?hh
require(__DIR__ . '/lib/Hashids/HashGenerator.hh');
require(__DIR__ . '/lib/Hashids/Hashids.hh');

$hashids = new \Hashids\Hashids(
	'm5QxG7MsGMJ2ZNwkv2xp2p'
);


define('MAX', $hashids->get_max_int_value());
$bench_against = $hashids->encode([$hashids->get_max_int_value() - 1000]);
$bench = microtime(true);

for ($i = 0; $i < 300000; $i++)
{
	$o = $hashids->decode($bench_against);
}

$bench = microtime(true) - $bench;
echo "Decoding of 300 000 hashes of a single number took {$bench} seconds. Don't forget that HHVM runs slower for the first time.";


