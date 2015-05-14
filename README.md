
![hashids](http://www.hashids.org.s3.amazonaws.com/public/img/hashids.png "Hashids")

======

Full Documentation
-------

A small Hack class to generate YouTube-like ids from numbers. Read documentation at [http://hashids.org/php](http://hashids.org/php)

Example Usage
-------

The simplest way to use Hashids:

```hack
<?hh

$hashids = new Hashids\Hashids();

$id = $hashids->encode([1, 2, 3]);
$numbers = $hashids->decode($id);

var_dump($id, $numbers);
```
	
	string(5) "laHquq"
	array(3) {
	  [0]=>
	  int(1)
	  [1]=>
	  int(2)
	  [2]=>
	  int(3)
	}
	
And an example with all the custom parameters provided (unique salt value, minimum id length, custom alphabet):

```hack
<?hh

$hashids = new Hashids\Hashids('this is my salt', 8, 'abcdefghij1234567890');

$id = $hashids->encode([1, 2, 3]);
$numbers = $hashids->decode($hash);

var_dump($id, $numbers);
```
	
	string(5) "514cdi42"
	array(3) {
	  [0]=>
	  int(1)
	  [1]=>
	  int(2)
	  [2]=>
	  int(3)
	}
	
Curses! #$%@
-------

This code was written with the intent of placing created ids in visible places - like the URL. Which makes it unfortunate if generated hashes accidentally formed a bad word.

Therefore, the algorithm tries to avoid generating most common English curse words. This is done by never placing the following letters next to each other:
	
	c, C, s, S, f, F, h, H, u, U, i, I, t, T
	
Big Numbers
-------

Each number passed to the constructor **cannot be negative**. Hashids `encode()` function will return an empty string if at least one of the numbers is out of bounds. Be sure to check for that -- no exception is thrown. Maximum number is **int(2147483647)** on 32-bit systems and **int(9223372036854775807)** on 64-bit.

Speed (FIXME: this section is about the PHP implementation)
-------

Even though speed is an important factor of every hashing algorithm, primary goal here was encoding several numbers at once while avoiding collisions.

On a *2.26 GHz Intel Core 2 Duo with 8GB of RAM*, it takes about:

1. **0.000093 seconds** to encode one number.
2. **0.000240 seconds** to decode one id (while ensuring that it's valid).
3. **0.493436 seconds** to generate **10,000** ids in a `for` loop.

On a *2.7 GHz Intel Core i7 with 16GB of RAM*, it takes roughly:

1. **0.000067 seconds** to encode one number.
2. **0.000113 seconds** to decode one id (and ensuring that it's valid).
3. **0.297426 seconds** to generate **10,000** ids in a `for` loop.

*Sidenote: The numbers tested with were relatively small -- if you increase them, the speed will obviously decrease.*

Difference between hashids.php and hashids.hack
-------
 - This Hack implementation is faster by 11% than the PHP implementation (if both of them are running on HHVM 3.7.0). Don't forget that there is a large room for the performance optimization in HHVM itself.
 - You must pass an ```array<int>``` to `encode()` function, i.e. ```$hashids->encode([100000])``` instead of ```$hashids->encode(100000)```.
 - It uses ```array<int>``` instead of ```string``` for internal operations. I don't remember why I decided so...

Notes
-------

- If you want to squeeze out even more performance, set a shorter alphabet. Hashes will be less random and longer, but calculating them will be faster.

Changelog
-------

**1.0.6.1**
- Initial Hack implementation (based on hashids.php by Ivan Akimov).

Contact
-------

* [@IvanAkimov](http://twitter.com/ivanakimov)
* Artem Chudinov: arzeth0@gmail.com

License
-------

MIT License. See the `LICENSE` file. You can use Hashids in open source projects and commercial products. Don't break the Internet. Kthxbye.
