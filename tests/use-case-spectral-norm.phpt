--TEST--
Use case: spectral norm
--FILE--
<?php 
/* The Computer Language Benchmarks Game
http://shootout.alioth.debian.org/

contributed by Isaac Gouy
modified by anon
*/


define('N', 500);

/**
 * @engine qb
 * @param float64		$i
 * @param float64		$j
 * @return float64
 */
function A($i, $j){
   return 1.0 / ( ( ( ($i+$j) * ($i+$j+1) ) >> 1 ) + $i + 1 );
}


/**
 * @engine qb
 * @param float64[N]	$v
 * @local float64[N]	$Av
 * @local float64		$.*
 * @return float64[N]
 */
function Av($v){
   for($i = 0; $i < count($v); ++$i) {
      $sum = 0.0;
      foreach($v as $j => $v_j) {
         $sum += A($i,$j) * $v_j;
      }
      $Av[$i] = $sum;
   }
   return $Av;
}


/**
 * @engine qb
 * @param float64[N]	$v
 * @local float64[N]	$Atv
 * @local float64		$.*
 * @return float64[N]
 */
function Atv($v){
   for($i = 0; $i < count($v); ++$i) {
      $sum = 0.0;
      foreach($v as $j=>$v_j) {
         $sum += A($j,$i) * $v_j;
      }
      $Atv[$i] = $sum;
   }
   return $Atv;
}

/**
 * @engine qb
 * @param float64[N]	$v
 * @local float64[N]	$tmp
 * @return float64[N]
 */
function AtAv($v){
   $tmp = Av($v);
   return Atv($tmp);
}

/**
 * @engine qb
 * @local float64[N]	$u
 * @local float64[N]	$v
 * @local float64		$.*
 * @return float64
 */
function calc() {
   $u = 1.0;

   for ($i=0; $i<10; $i++){
      $v = AtAv($u);
      $u = AtAv($v);
   }

   $vBv = 0.0;
   $vv = 0.0;
   $i = 0;
   foreach($v as $val) {
      $vBv += $u[$i]*$val;
      $vv += $val*$val;
      ++$i;
   }
   return sqrt($vBv/$vv);
}

qb_compile();
printf("%0.9f\n", calc());

?>
--EXPECT--
1.274224116
