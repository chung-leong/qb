--TEST--
Use case: mandelbrot
--FILE--
<?php

/* The Computer Language Benchmarks Game
   http://shootout.alioth.debian.org/
   contributed by Thomas GODART (based on Greg Buchholz's C program) 
   modified by anon
*/

/**
 * A test function
 * 
 * @engine	qb
 * @param	int32	$[hw]
 * @local	int32	$[xy]
 * @local	char	$byte_acc
 * @local	int32	$bit_num
 * @local	float64	$.*
 * 
 * @return	void
 * 
 */
function calc($h, $w) {
   $bit_num = 128;
   $byte_acc = 0;

   $yfac = 2.0 / $h;
   $xfac = 2.0 / $w;

   for ($y = 0 ; $y < $h ; ++$y)
   {
      $Ci = $y * $yfac - 1.0;

      for ($x = 0 ; $x < $w ; ++$x)
      {
         $Zr = 0; $Zi = 0; $Tr = 0; $Ti = 0.0;

         $Cr = $x * $xfac - 1.5;

         do {
            for ($i = 0 ; $i < 50 ; ++$i)
            {
               $Zi = 2.0 * $Zr * $Zi + $Ci;
               $Zr = $Tr - $Ti + $Cr;
               $Tr = $Zr * $Zr;
               if (($Tr+($Ti = $Zi * $Zi)) > 4.0) break 2;
            }
            $byte_acc += $bit_num;
         } while (FALSE);

         if ($bit_num == 1) {
            echo $byte_acc;
            $bit_num = 128;
            $byte_acc = 0;
         } else {
            $bit_num >>= 1;
         }
      }
      if ($bit_num != 128) {
         echo $byte_acc;
         $bit_num = 128;
         $byte_acc = 0;
      }
   }
}

ob_start();

$h = 200;
$w = $h;

printf ("P4\n%d %d\n", $w, $h);

calc($h, $w);

echo md5(ob_get_clean()), "\n";

?>
--EXPECT--
cc65e64bd553ed18896de1dfe7fae3e5
