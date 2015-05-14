<?hh
require(__DIR__ . '/lib/Hashids/HashGenerator.hh');
require(__DIR__ . '/lib/Hashids/Hashids.hh');


$hashids = new \Hashids\Hashids(
	'm5QxG7MsGMJ2ZNwkv2xp2p'
);


define('MAX', $hashids->get_max_int_value());
$bench = microtime(true);

for ($i = 0; $i < 300000; $i++)
{
	$o = $hashids->encode([MAX - $i]);
}

$bench = microtime(true) - $bench;
echo "Encoding of 300 000 hashes of a different numbers took {$bench} seconds. Don't forget that HHVM runs slower for the first time.";

