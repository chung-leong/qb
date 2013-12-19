--TEST--
Use case: n-body
--FILE--
<?php
/* The Computer Language Benchmarks Game
http://shootout.alioth.debian.org/

contributed by anon
modified by Sergey Khripunov
*/

$pi = 3.141592653589793;
$solar_mass = 4 * $pi * $pi;
$days_per_year = 365.24;

$bodies = array(array(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, $solar_mass), //Sun
		array(4.84143144246472090E+00, // Jupiter
		      -1.16032004402742839E+00,
		      -1.03622044471123109E-01,
		      1.66007664274403694E-03 * $days_per_year,
		      7.69901118419740425E-03 * $days_per_year,
		      -6.90460016972063023E-05 * $days_per_year,
		      9.54791938424326609E-04 * $solar_mass),
		array(8.34336671824457987E+00, // Saturn
		      4.12479856412430479E+00,
		      -4.03523417114321381E-01,
		      -2.76742510726862411E-03 * $days_per_year,
		      4.99852801234917238E-03 * $days_per_year,
		      2.30417297573763929E-05 * $days_per_year,
		      2.85885980666130812E-04 * $solar_mass),
		array(1.28943695621391310E+01, // Uranus
		      -1.51111514016986312E+01,
		      -2.23307578892655734E-01,
		      2.96460137564761618E-03 * $days_per_year,
		      2.37847173959480950E-03 * $days_per_year,
		      -2.96589568540237556E-05 * $days_per_year,
		      4.36624404335156298E-05 * $solar_mass),
		array(1.53796971148509165E+01, // Neptune
		      -2.59193146099879641E+01,
		      1.79258772950371181E-01,
		      2.68067772490389322E-03 * $days_per_year,
		      1.62824170038242295E-03 * $days_per_year,
		      -9.51592254519715870E-05 * $days_per_year,
		      5.15138902046611451E-05 * $solar_mass));

// offset_momentum
$px = $py = $pz = 0.0;
foreach($bodies as &$e) {
    $px += $e[3] * $e[6]; 
    $py += $e[4] * $e[6]; 
    $pz += $e[5] * $e[6];
} 
$bodies[0][3] =- $px / $solar_mass;
$bodies[0][4] =- $py / $solar_mass;
$bodies[0][5] =- $pz / $solar_mass;

$n = 500000;

/**
 * 
 * @engine	qb
 * @param	float64[5][7]	$b
 * @local	float64[7]		$b1
 * @local	float64[7]		$b2
 * @local	uint32			$[ijm]
 * @local	float64			$.*
 * 
 * @return	float64
 * 
 */
function energy(&$b) {
   $e = 0.0;
   for($i = 0, $m = sizeof($b); $i < $m; $i++) {
       $b1 = $b[$i]; 
       $e += 0.5 * $b1[6] * ($b1[3] * $b1[3] + $b1[4] * $b1[4] + $b1[5] * $b1[5]);
       for($j = $i + 1; $j < $m; $j++) {
	  $b2 = $b[$j];
	  $dx = $b1[0] - $b2[0]; 
	  $dy = $b1[1] - $b2[1]; 
	  $dz = $b1[2] - $b2[2];
	  $e -= ($b1[6] * $b2[6]) / sqrt($dx * $dx + $dy * $dy + $dz * $dz);
       }
   }
   return $e;
}

/**
 * 
 * @engine	qb
 * @param	uint32				$n
 * @param	float64[5][7]		$bodies
 * @local	uint32				$[ijkmn]
 * @local	float64				$.*
 * 
 * @return	float64
 * 
 */
function calc($n, &$bodies) {
	$i = 0;
	do {
		for($k = 0; $k < count($bodies); $k++) {
			for($m = $k + 1; $m < count($bodies); $m++) {
				$dx = $bodies[$k][0] - $bodies[$m][0];
				$dy = $bodies[$k][1] - $bodies[$m][1];
				$dz = $bodies[$k][2] - $bodies[$m][2];
			
				$dist = sqrt($dx * $dx + $dy * $dy + $dz * $dz);
				$mag = 0.01 / ($dist * $dist * $dist);
				$mag_a = $bodies[$k][6] * $mag;
				$mag_b = $bodies[$m][6] * $mag;
			
				$bodies[$k][3] -= $dx * $mag_b;
				$bodies[$k][4] -= $dy * $mag_b;
				$bodies[$k][5] -= $dz * $mag_b;
					
				$bodies[$m][3] += $dx * $mag_a;
				$bodies[$m][4] += $dy * $mag_a;
				$bodies[$m][5] += $dz * $mag_a;
			}
		}
		for($j = 0; $j < count($bodies); $j++) {
			$bodies[$j][0] += 0.01 * $bodies[$j][3];
			$bodies[$j][1] += 0.01 * $bodies[$j][4];
			$bodies[$j][2] += 0.01 * $bodies[$j][5];
		}
	} while(++$i < $n);
}

printf("%0.9f\n", energy($bodies));

calc($n, $bodies);

printf("%0.9f\n", energy($bodies));

?>
--EXPECT--
-0.169075164
-0.169096567
