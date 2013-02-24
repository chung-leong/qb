<?php

class QBUnpackHandler extends QBHandler {
	protected $byteOrder;

	public function __construct($name, $operandType, $addressMode, $byteOrder) {
		parent::__construct($name, $operandType, $addressMode);
		$this->byteOrder = $byteOrder;
	}
	
	public function getOperandType($i) {
		switch($i) {
			case 1:	return "U08";
			case 2: return $this->operandType;
		}
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return "ARR";
			case 2: return $this->addressMode;
		}
	}
	
	public function getAction() {
		static $msvcMacros = array(
			16 => '_byteswap_ushort',
			32 => '_byteswap_ulong',
			64 => '_byteswap_uint64',
		);
		static $gccMacros = array(
			16 => '__builtin_bswap16',
			32 => '__builtin_bswap32',
			64 => '__builtin_bswap64',
		);
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$width = intval(substr($type, 1));
		$lines = array();
		$macro = (self::$compiler == 'MSVC') ? $msvcMacros[$width] : $gccMacros[$width];
		$lines[] = "#ifdef QB_{$this->byteOrder}";
		$lines[] = "res = *(($cType *) op1_ptr);";
		$lines[] = "#else";
		$lines[] = "*((uint{$width}_t *) &res) = $macro(*((uint{$width}_t *) op1_ptr));";
		$lines[] = "#endif";
		return $lines;
	}
}

?>