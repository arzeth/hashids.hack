<?hh
require(__DIR__ . '/vendor/autoload.php');

$hashids = new \Hashids\Hashids(
	'm5QxG7MsGMJ2ZNwkv2xp2p'
);


$MAX = $hashids->get_max_int_value();
$bench_against = $hashids->encode([$MAX - 1000]);
$bench = microtime(true);

for ($i = 0; $i < 300000; $i++)
{
	$o = $hashids->decode($bench_against);
}

$bench = microtime(true) - $bench;
echo "Decoding of 300 000 hashes of a single number took {$bench} seconds. Don't forget that HHVM runs slower for the first time.";


