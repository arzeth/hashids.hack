<?hh
require(__DIR__ . '/vendor/autoload.php');

$hashids = new \Hashids\Hashids(
	'm5QxG7MsGMJ2ZNwkv2xp2p'
);


$MAX = $hashids->get_max_int_value();
$bench = microtime(true);

for ($i = 0; $i < 300000; $i++)
{
	$o = $hashids->encode([$MAX - $i]);
}

$bench = microtime(true) - $bench;
echo "Encoding of 300 000 hashes of a different numbers took {$bench} seconds. Don't forget that HHVM runs slower for the first time.";

