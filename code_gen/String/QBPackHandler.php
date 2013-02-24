<?php

class QBPackHandler extends QBHandler {
	protected $byteOrder;

	public function __construct($name, $operandType, $addressMode, $byteOrder) {
		parent::__construct($name, $operandType, $addressMode);
		$this->byteOrder = $byteOrder;
	}

	public function getOperandType($i) {
		switch($i) {
			case 1: return $this->operandType;
			case 2:	return "U08";
		}
	}

	public function getOperandAddressMode($i) {
		switch($i) {
			case 1: return $this->addressMode;
			case 2: return "ARR";
		}
	}

	public function getResultSizePossibilities() {
		$type = $this->getOperandType(1);
		$size = substr($type, 1) / 8;
		return $size;
	}

	protected function getScalarExpression() {
		$type = $this->getOperandType(1);
		$cType = $this->getOperandCType(1);
		$lines = array();
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
		$width = intval(substr($type, 1));
		$macro = (self::$compiler == 'MSVC') ? $msvcMacros[$width] : $gccMacros[$width];
		$lines = array();
		$lines[] = "#ifdef QB_{$this->byteOrder}";
		$lines[] = "*(($cType *) &res) = op1;";
		$lines[] = "#else";
		$lines[] = "{";
		$lines[] = "$cType v = op1;\n";
		$lines[] = "*((uint{$width}_t *) &res) = $macro(*((uint{$width}_t *) &v));";
		$lines[] = "}";
		$lines[] = "#endif";
		return $lines;
	}
}

?>