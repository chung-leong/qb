--TEST--
Use case: Keccak
--FILE--
<?php

/**
 * 
 * @engine	qb
 * @param	uint64[25]	$state
 * @param	uint64[18]	$in
 * @static	uint64[24]	$KeccakF_RoundConstants
 * @local	uint32	$i
 * @local	uint64	$.*
 * 
 * @return	void
 * 
 */
function KeccakF(&$state, $in) {
	static $KeccakF_RoundConstants = array(
		array(	0x00000000, 0x00000001	),
		array(	0x00000000, 0x00008082	),
		array(	0x80000000, 0x0000808a	),
		array(	0x80000000, 0x80008000	),
		array(	0x00000000, 0x0000808b	),
		array(	0x00000000, 0x80000001	),
		array(	0x80000000, 0x80008081	),
		array(	0x80000000, 0x00008009	),
		array(	0x00000000, 0x0000008a	),
		array(	0x00000000, 0x00000088	),
		array(	0x00000000, 0x80008009	),
		array(	0x00000000, 0x8000000a	),
		array(	0x00000000, 0x8000808b	),
		array(	0x80000000, 0x0000008b	),
		array(	0x80000000, 0x00008089	),
		array(	0x80000000, 0x00008003	),
		array(	0x80000000, 0x00008002	),
		array(	0x80000000, 0x00000080	),
		array(	0x00000000, 0x0000800a	),
		array(	0x80000000, 0x8000000a	),
		array(	0x80000000, 0x80008081	),
		array(	0x80000000, 0x00008080	),
		array(	0x00000000, 0x80000001	),
		array(	0x80000000, 0x80008008	),
	);

	for($i = 0; $i < count($in); $i++) {
	    $state[$i] ^= $in[$i];
	}

	//copyFromState(A, state)
	$Aba = $state[ 0];
	$Abe = $state[ 1];
	$Abi = $state[ 2];
	$Abo = $state[ 3];
	$Abu = $state[ 4];
	$Aga = $state[ 5];
	$Age = $state[ 6];
	$Agi = $state[ 7];
	$Ago = $state[ 8];
	$Agu = $state[ 9];
	$Aka = $state[10];
	$Ake = $state[11];
	$Aki = $state[12];
	$Ako = $state[13];
	$Aku = $state[14];
	$Ama = $state[15];
	$Ame = $state[16];
	$Ami = $state[17];
	$Amo = $state[18];
	$Amu = $state[19];
	$Asa = $state[20];
	$Ase = $state[21];
	$Asi = $state[22];
	$Aso = $state[23];
	$Asu = $state[24];

	for($i = 0; $i < 24; $i += 2) {
		//    prepareTheta
		$BCa = $Aba^$Aga^$Aka^$Ama^$Asa;
		$BCe = $Abe^$Age^$Ake^$Ame^$Ase;
		$BCi = $Abi^$Agi^$Aki^$Ami^$Asi;
		$BCo = $Abo^$Ago^$Ako^$Amo^$Aso;
		$BCu = $Abu^$Agu^$Aku^$Amu^$Asu;
		
		//thetaRhoPiChiIotaPrepareTheta(round  , A, E)
		$Da = $BCu^(($BCe<<1)^($BCe>>(64-1)));
		$De = $BCa^(($BCi<<1)^($BCi>>(64-1)));
		$Di = $BCe^(($BCo<<1)^($BCo>>(64-1)));
		$Do = $BCi^(($BCu<<1)^($BCu>>(64-1)));
		$Du = $BCo^(($BCa<<1)^($BCa>>(64-1)));
		
		$Aba ^= $Da;
		$BCa = $Aba;
		$Age ^= $De;
		$BCe = (($Age<<44)^($Age>>(64-44)));
		$Aki ^= $Di;
		$BCi = (($Aki<<43)^($Aki>>(64-43)));
		$Amo ^= $Do;
		$BCo = (($Amo<<21)^($Amo>>(64-21)));
		$Asu ^= $Du;
		$BCu = (($Asu<<14)^($Asu>>(64-14)));
		$Eba =   $BCa ^((~$BCe)&  $BCi );
		$Eba ^= $KeccakF_RoundConstants[$i];
		$Ebe =   $BCe ^((~$BCi)&  $BCo );
		$Ebi =   $BCi ^((~$BCo)&  $BCu );
		$Ebo =   $BCo ^((~$BCu)&  $BCa );
		$Ebu =   $BCu ^((~$BCa)&  $BCe );
		
		$Abo ^= $Do;
		$BCa = (($Abo<<28)^($Abo>>(64-28)));
		$Agu ^= $Du;
		$BCe = (($Agu<<20)^($Agu>>(64-20)));
		$Aka ^= $Da;
		$BCi = (($Aka<< 3)^($Aka>>(64- 3)));
		$Ame ^= $De;
		$BCo = (($Ame<<45)^($Ame>>(64-45)));
		$Asi ^= $Di;
		$BCu = (($Asi<<61)^($Asi>>(64-61)));
		$Ega =   $BCa ^((~$BCe)&  $BCi );
		$Ege =   $BCe ^((~$BCi)&  $BCo );
		$Egi =   $BCi ^((~$BCo)&  $BCu );
		$Ego =   $BCo ^((~$BCu)&  $BCa );
		$Egu =   $BCu ^((~$BCa)&  $BCe );
		
		$Abe ^= $De;
		$BCa = (($Abe<< 1)^($Abe>>(64- 1)));
		$Agi ^= $Di;
		$BCe = (($Agi<< 6)^($Agi>>(64- 6)));
		$Ako ^= $Do;
		$BCi = (($Ako<<25)^($Ako>>(64-25)));
		$Amu ^= $Du;
		$BCo = (($Amu<< 8)^($Amu>>(64- 8)));
		$Asa ^= $Da;
		$BCu = (($Asa<<18)^($Asa>>(64-18)));
		$Eka =   $BCa ^((~$BCe)&  $BCi );
		$Eke =   $BCe ^((~$BCi)&  $BCo );
		$Eki =   $BCi ^((~$BCo)&  $BCu );
		$Eko =   $BCo ^((~$BCu)&  $BCa );
		$Eku =   $BCu ^((~$BCa)&  $BCe );
		
		$Abu ^= $Du;
		$BCa = (($Abu<<27)^($Abu>>(64-27)));
		$Aga ^= $Da;
		$BCe = (($Aga<<36)^($Aga>>(64-36)));
		$Ake ^= $De;
		$BCi = (($Ake<<10)^($Ake>>(64-10)));
		$Ami ^= $Di;
		$BCo = (($Ami<<15)^($Ami>>(64-15)));
		$Aso ^= $Do;
		$BCu = (($Aso<<56)^($Aso>>(64-56)));
		$Ema =   $BCa ^((~$BCe)&  $BCi );
		$Eme =   $BCe ^((~$BCi)&  $BCo );
		$Emi =   $BCi ^((~$BCo)&  $BCu );
		$Emo =   $BCo ^((~$BCu)&  $BCa );
		$Emu =   $BCu ^((~$BCa)&  $BCe );
		
		$Abi ^= $Di;
		$BCa = (($Abi<<62)^($Abi>>(64-62)));
		$Ago ^= $Do;
		$BCe = (($Ago<<55)^($Ago>>(64-55)));
		$Aku ^= $Du;
		$BCi = (($Aku<<39)^($Aku>>(64-39)));
		$Ama ^= $Da;
		$BCo = (($Ama<<41)^($Ama>>(64-41)));
		$Ase ^= $De;
		$BCu = (($Ase<<2)^($Ase>>(64-2)));
		$Esa =   $BCa ^((~$BCe)&  $BCi );
		$Ese =   $BCe ^((~$BCi)&  $BCo );
		$Esi =   $BCi ^((~$BCo)&  $BCu );
		$Eso =   $BCo ^((~$BCu)&  $BCa );
		$Esu =   $BCu ^((~$BCa)&  $BCe );
		
		//    prepareTheta
		$BCa = $Eba^$Ega^$Eka^$Ema^$Esa;
		$BCe = $Ebe^$Ege^$Eke^$Eme^$Ese;
		$BCi = $Ebi^$Egi^$Eki^$Emi^$Esi;
		$BCo = $Ebo^$Ego^$Eko^$Emo^$Eso;
		$BCu = $Ebu^$Egu^$Eku^$Emu^$Esu;
		
		//thetaRhoPiChiIotaPrepareTheta(round+1, E, A)
		$Da = $BCu^(($BCe<<1)^($BCe>>(64-1)));
		$De = $BCa^(($BCi<<1)^($BCi>>(64-1)));
		$Di = $BCe^(($BCo<<1)^($BCo>>(64-1)));
		$Do = $BCi^(($BCu<<1)^($BCu>>(64-1)));
		$Du = $BCo^(($BCa<<1)^($BCa>>(64-1)));
		
		$Eba ^= $Da;
		$BCa = $Eba;
		$Ege ^= $De;
		$BCe = (($Ege<<44)^($Ege>>(64-44)));
		$Eki ^= $Di;
		$BCi = (($Eki<<43)^($Eki>>(64-43)));
		$Emo ^= $Do;
		$BCo = (($Emo<<21)^($Emo>>(64-21)));
		$Esu ^= $Du;
		$BCu = (($Esu<<14)^($Esu>>(64-14)));
		$Aba =   $BCa ^((~$BCe)&  $BCi );
		$Aba ^= $KeccakF_RoundConstants[$i+1];
		$Abe =   $BCe ^((~$BCi)&  $BCo );
		$Abi =   $BCi ^((~$BCo)&  $BCu );
		$Abo =   $BCo ^((~$BCu)&  $BCa );
		$Abu =   $BCu ^((~$BCa)&  $BCe );
		
		$Ebo ^= $Do;
		$BCa = (($Ebo<<28)^($Ebo>>(64-28)));
		$Egu ^= $Du;
		$BCe = (($Egu<<20)^($Egu>>(64-20)));
		$Eka ^= $Da;
		$BCi = (($Eka<< 3)^($Eka>>(64- 3)));
		$Eme ^= $De;
		$BCo = (($Eme<<45)^($Eme>>(64-45)));
		$Esi ^= $Di;
		$BCu = (($Esi<<61)^($Esi>>(64-61)));
		$Aga =   $BCa ^((~$BCe)&  $BCi );
		$Age =   $BCe ^((~$BCi)&  $BCo );
		$Agi =   $BCi ^((~$BCo)&  $BCu );
		$Ago =   $BCo ^((~$BCu)&  $BCa );
		$Agu =   $BCu ^((~$BCa)&  $BCe );
		
		$Ebe ^= $De;
		$BCa = (($Ebe<< 1)^($Ebe>>(64- 1)));
		$Egi ^= $Di;
		$BCe = (($Egi<< 6)^($Egi>>(64- 6)));
		$Eko ^= $Do;
		$BCi = (($Eko<<25)^($Eko>>(64-25)));
		$Emu ^= $Du;
		$BCo = (($Emu<< 8)^($Emu>>(64- 8)));
		$Esa ^= $Da;
		$BCu = (($Esa<<18)^($Esa>>(64-18)));
		$Aka =   $BCa ^((~$BCe)&  $BCi );
		$Ake =   $BCe ^((~$BCi)&  $BCo );
		$Aki =   $BCi ^((~$BCo)&  $BCu );
		$Ako =   $BCo ^((~$BCu)&  $BCa );
		$Aku =   $BCu ^((~$BCa)&  $BCe );
		
		$Ebu ^= $Du;
		$BCa = (($Ebu<<27)^($Ebu>>(64-27)));
		$Ega ^= $Da;
		$BCe = (($Ega<<36)^($Ega>>(64-36)));
		$Eke ^= $De;
		$BCi = (($Eke<<10)^($Eke>>(64-10)));
		$Emi ^= $Di;
		$BCo = (($Emi<<15)^($Emi>>(64-15)));
		$Eso ^= $Do;
		$BCu = (($Eso<<56)^($Eso>>(64-56)));
		$Ama =   $BCa ^((~$BCe)&  $BCi );
		$Ame =   $BCe ^((~$BCi)&  $BCo );
		$Ami =   $BCi ^((~$BCo)&  $BCu );
		$Amo =   $BCo ^((~$BCu)&  $BCa );
		$Amu =   $BCu ^((~$BCa)&  $BCe );
		
		$Ebi ^= $Di;
		$BCa = (($Ebi<<62)^($Ebi>>(64-62)));
		$Ego ^= $Do;
		$BCe = (($Ego<<55)^($Ego>>(64-55)));
		$Eku ^= $Du;
		$BCi = (($Eku<<39)^($Eku>>(64-39)));
		$Ema ^= $Da;
		$BCo = (($Ema<<41)^($Ema>>(64-41)));
		$Ese ^= $De;
		$BCu = (($Ese<<2)^($Ese>>(64-2)));
		$Asa =   $BCa ^((~$BCe)&  $BCi );
		$Ase =   $BCe ^((~$BCi)&  $BCo );
		$Asi =   $BCi ^((~$BCo)&  $BCu );
		$Aso =   $BCo ^((~$BCu)&  $BCa );
		$Asu =   $BCu ^((~$BCa)&  $BCe );
	}

	//copyToState(state, A)
	$state[ 0] = $Aba;
	$state[ 1] = $Abe;
	$state[ 2] = $Abi;
	$state[ 3] = $Abo;
	$state[ 4] = $Abu;
	$state[ 5] = $Aga;
	$state[ 6] = $Age;
	$state[ 7] = $Agi;
	$state[ 8] = $Ago;
	$state[ 9] = $Agu;
	$state[10] = $Aka;
	$state[11] = $Ake;
	$state[12] = $Aki;
	$state[13] = $Ako;
	$state[14] = $Aku;
	$state[15] = $Ama;
	$state[16] = $Ame;
	$state[17] = $Ami;
	$state[18] = $Amo;
	$state[19] = $Amu;
	$state[20] = $Asa;
	$state[21] = $Ase;
	$state[22] = $Asi;
	$state[23] = $Aso;
	$state[24] = $Asu;
}

/**
 * 
 * @engine	qb
 * @param	uint64[25]		$state
 * @param	uint64[][18]	$blocks
 * @local	uint64[18]	$block
 * @local	uint32	$i
 * 
 * @return	void
 * 
 */
function Keccak(&$state, $blocks) {
	foreach($blocks as $block) {
		KeccakF($state, $block);
	}
}

function calc($data) {
	$len = strlen($data);
	$extra = $len % 144;
	$data .= "\x01" . str_repeat("\x00", 142 - $extra) . "\x80";
	$state = str_repeat("\x00", 25 * 8);
	Keccak($state, $data);
	for($i = 0; $i < 28; $i++) {
		printf("%02x", ord($state[$i]));
	}
	echo "\n";
}

calc("");
calc("The quick brown fox jumps over the lazy dog");

?>
--EXPECT--
f71837502ba8e10837bdd8d365adb85591895602fc552b48b7390abd
310aee6b30c47350576ac2873fa89fd190cdc488442f3ef654cf23fe