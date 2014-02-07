--TEST--
Logical short-circuiting test
--FILE--
<?php

/**
 * @engine	qb
 * @return	bool
 * 
 */
function A() {
	echo "A() = true\n";
	return true;
}

/**
 * @engine	qb
 * @return	bool
 * 
 */
function B() {
	echo "B() = true\n";
	return true;
}

/**
 * @engine	qb
 * @return	bool
 * 
 */
function C() {
	echo "C() = false\n";
	return false;
}

/**
 * @engine	qb
 * @return	bool
 * 
 */
function D() {
	echo "D() = false\n";
	return false;
}


/**
 * @engine	qb
 * @return	void
 * 
 */
function test_function() {
	if(A() && B()) {
		echo "A() && B() = true\n";
	} else {
		echo "A() && B() = false\n";
	}
	
	if(A() || B()) {
		echo "A() || B() = true\n";
	} else {
		echo "A() || B() = false\n";
	}
	if(A() || B() || C()) {
		echo "A() || B() || C() = true\n";
	} else {
		echo "A() || B() || C() = false\n";
	}
	if(A() && B() && C()) {
		echo "A() && B() && C() = true\n";
	} else {
		echo "A() && B() && C() = false\n";
	}

	if(A() && B() || C()) {
		echo "A() && B() || C() = true\n";
	} else {
		echo "A() && B() || C() = false\n";
	}

	if((C() && D()) && (A() && B())) {
		echo "(C() && D()) && (A() && B()) = true\n";
	} else {
		echo "(C() && D()) && (A() && B()) = false\n";
	}

	if((C() || D()) && (A() && B())) {
		echo "(C() || D()) && (A() && B()) = true\n";
	} else {
		echo "(C() || D()) && (A() && B()) = false\n";
	}
	
	if((C() || D()) || (A() && B())) {
		echo "(C() || D()) || (A() && B()) = true\n";
	} else {
		echo "(C() || D()) || (A() && B()) = false\n";
	}	
	
	if((C() || D()) || (A() || B())) {
		echo "(C() || D()) || (A() || B()) = true\n";
	} else {
		echo "(C() || D()) || (A() || B()) = false\n";
	}	
}

test_function();

?>
--EXPECT--
A() = true
B() = true
A() && B() = true
A() = true
A() || B() = true
A() = true
A() || B() || C() = true
A() = true
B() = true
C() = false
A() && B() && C() = false
A() = true
B() = true
A() && B() || C() = true
C() = false
(C() && D()) && (A() && B()) = false
C() = false
D() = false
(C() || D()) && (A() && B()) = false
C() = false
D() = false
A() = true
B() = true
(C() || D()) || (A() && B()) = true
C() = false
D() = false
A() = true
(C() || D()) || (A() || B()) = true
