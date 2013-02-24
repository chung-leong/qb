/*
 * qb_version.h
 *
 *  Created on: Feb 20, 2013
 *      Author: cleong
 */

#ifndef QB_VERSION_H_
#define QB_VERSION_H_

#define QB_MAJOR_VERSION			1
#define QB_MINOR_VERSION			1

#define QB_RELEASE_NAME_1			"Agnieszka"
#define QB_RELEASE_NAME_2			"Basia"
#define QB_RELEASE_NAME_3			"Cyntia"
#define QB_RELEASE_NAME_4			"Dorota"
#define QB_RELEASE_NAME_5			"Ewa"
#define QB_RELEASE_NAME_6			"Franciszka"
#define QB_RELEASE_NAME_7			"Gabriela"
#define QB_RELEASE_NAME_8			"Helena"
#define QB_RELEASE_NAME_9			"Irena"
#define QB_RELEASE_NAME_10			"Joanna"
#define QB_RELEASE_NAME_11			"Karolina"
#define QB_RELEASE_NAME_12			"Luiza"
#define QB_RELEASE_NAME_13			"Malgosia"
#define QB_RELEASE_NAME_14			"Natasza"
#define QB_RELEASE_NAME_15			"Otylia"
#define QB_RELEASE_NAME_16			"Paulina"
#define QB_RELEASE_NAME_17			"Renata"
#define QB_RELEASE_NAME_18			"Sonia"
#define QB_RELEASE_NAME_19			"Teresa"
#define QB_RELEASE_NAME_20			"Urszula"
#define QB_RELEASE_NAME_21			"Wioletta"
#define QB_RELEASE_NAME_22			"Zuzanna"
#define QB_RELEASE_NAME_23			"Agata"
#define QB_RELEASE_NAME_24			"Beata"
#define QB_RELEASE_NAME_25			"Czeslawa"
#define QB_RELEASE_NAME_26			"Dagmara"
#define QB_RELEASE_NAME_27			"Edyta"
#define QB_RELEASE_NAME_28			"Felicja"
#define QB_RELEASE_NAME_29			"Grazyna"
#define QB_RELEASE_NAME_30			"Halina"
#define QB_RELEASE_NAME_31			"Izabela"
#define QB_RELEASE_NAME_32			"Jadwiga"
#define QB_RELEASE_NAME_33			"Kasia"
#define QB_RELEASE_NAME_34			"Lena"
#define QB_RELEASE_NAME_35			"Marysia"
#define QB_RELEASE_NAME_36			"Nina"
#define QB_RELEASE_NAME_37			"Olga"
#define QB_RELEASE_NAME_38			"Patrycja"
#define QB_RELEASE_NAME_39			"Rebeka"
#define QB_RELEASE_NAME_40			"Sylwia"
#define QB_RELEASE_NAME_41			"Tamara"
#define QB_RELEASE_NAME_42			"Ulryka"
#define QB_RELEASE_NAME_43			"Weronika"
#define QB_RELEASE_NAME_44			"Zbygniewa"

#define __QB_RELEASE_NAME(n)		QB_RELEASE_NAME_##n
#define _QB_RELEASE_NAME(n)			__QB_RELEASE_NAME(n)
#define QB_RELEASE_NAME				_QB_RELEASE_NAME(QB_MAJOR_VERSION)

#define QB_REVISION_INITIAL_1		"A."
#define QB_REVISION_INITIAL_2		"B."
#define QB_REVISION_INITIAL_3		"C."
#define QB_REVISION_INITIAL_4		"D."
#define QB_REVISION_INITIAL_5		"E."
#define QB_REVISION_INITIAL_6		"F."
#define QB_REVISION_INITIAL_7		"G."
#define QB_REVISION_INITIAL_8		"H."
#define QB_REVISION_INITIAL_9		"I."
#define QB_REVISION_INITIAL_10		"J."
#define QB_REVISION_INITIAL_11		"K."
#define QB_REVISION_INITIAL_12		"L."
#define QB_REVISION_INITIAL_13		"M."
#define QB_REVISION_INITIAL_14		"N."
#define QB_REVISION_INITIAL_15		"O."
#define QB_REVISION_INITIAL_16		"P."
#define QB_REVISION_INITIAL_17		"R."
#define QB_REVISION_INITIAL_18		"S."
#define QB_REVISION_INITIAL_19		"T."
#define QB_REVISION_INITIAL_20		"U."
#define QB_REVISION_INITIAL_21		"W."
#define QB_REVISION_INITIAL_22		"Z."

#define __QB_REVISION_INITIAL(n)	QB_REVISION_INITIAL_##n
#define _QB_REVISION_INITIAL(n)		__QB_REVISION_INITIAL(n)
#define QB_REVISION_INITIAL			_QB_REVISION_INITIAL(QB_MINOR_VERSION)

#define QB_FULL_RELEASE_NAME		QB_RELEASE_NAME" "QB_REVISION_INITIAL

#endif /* QB_VERSION_H_ */
