/*
 * bc-robert.c
 * 
 * Thu Oct 17 2012 Robert D. Wei <robertmodesty@gmail.com>
 * 
 * Version: 0.1.1
 * 
 * License: GPL 2.0
 * 
 */
/* GNU-EFI porting as SetA.c by Wei-Lun Chao <bluebat@member.fsf.org>, 2017 */

#include <efi.h>
#include <efilib.h>

#define deEXPRESS AddSubtract
#define deUNIT PeelParenthes
#define deNUMBER GetNumber

#define ADD_SUB_L deEXPRESS
#define ADD_SUB_R MultiplieDivise
#define MUL_DIV_L MultiplieDivise
#define MUL_DIV_R ToThePowerOf
#define POW_BASE deUNIT
#define POW_EXP ToThePowerOf
#define IN_PRNTH deEXPRESS
#define NO_PRNTH deNUMBER

void LinkAgrements (CHAR16* ExpressString, INTN argc, CHAR16* argv[] ) ;
CHAR16* RemoveSpaces (CHAR16* ExpressString) ;
INTN AddSubtract (const CHAR16* SubString, const CHAR16* StringTail) ;
INTN MultiplieDivise (const CHAR16* SubString, const CHAR16* StringTail) ;
INTN ToThePowerOf (const CHAR16* SubString, const CHAR16* StringTail) ;
INTN PeelParenthes (const CHAR16* SubString, const CHAR16* StringTail) ;
INTN GetNumber (const CHAR16* SubString, const CHAR16* StringTail) ;

EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  INTN argc ;
  CHAR16 **argv ;
  InitializeLib(ImageHandle, SystemTable) ;
  argc = GetShellArgcArgv(ImageHandle, &argv) ;

	INTN Length = 0, Index, ExpressValue ;
	for ( Index = 1 ; Index < argc ; Index++ )
		Length += StrLen ( argv [Index] ) ;
	CHAR16 ExpressString [ Length + 1 ] ;
	CHAR16* StringTail ;

	LinkAgrements ( ExpressString, argc, argv ) ;

	StringTail = RemoveSpaces ( ExpressString ) ;

	if ( StringTail >= ExpressString ) {
		ExpressValue = deEXPRESS ( ExpressString, StringTail ) ;
		if ( ExpressValue < 0 )
			Print ( L"-%d\n" , -1 * ExpressValue) ;
		else
			Print ( L"%d\n" , ExpressValue) ;
		return EFI_SUCCESS ;
	} else {
		Print ( L"Usage: SetA ARITH_EXPR\n" ) ;
		return EFI_INVALID_PARAMETER ;
	}	
}

void LinkAgrements (CHAR16* ExpressString, INTN argc, CHAR16* argv[] ) {
	CHAR16 *Origin, *Target = ExpressString ;
	INTN Index ;

	for ( Index = 1 ; Index < argc ; Index++ ) {
		Origin = argv [Index] ;
		while ( *Origin != '\0' ) {
			*Target = *Origin ;
		Origin++ ;	Target++ ;	}
	}
	*Target = '\0' ;
}

CHAR16* RemoveSpaces (CHAR16* ExpressString) {
	CHAR16 *Origin = ExpressString , *Target = Origin - 1 ;

	while ( *Origin != '\0' ) {
		if ( *Origin != ' ' && *Origin!='\n'
				&& *Origin!='\'' && *Origin!='\"' ) {
			Target++ ;
			*Target = *Origin ;
		}
	Origin++ ; }
	
	return Target ; /* Return the new tail of the string. */
}

INTN AddSubtract (const CHAR16* SubString, const CHAR16* StringTail) {
	const CHAR16 *Point ;
	INTN Chack = 0 ;

	for ( Point = StringTail ; Point > SubString ; Point-- ) {
		/* If there are, skip characters between parenthes pair.  */
		if ( *Point == ')' ) {
			Chack++ ;
			while ( Point > SubString && Chack != 0 ) {
				Point-- ;
				switch ( *Point ) {
					case ')' : Chack++ ; break ;
					case '(' : Chack-- ; break ;
				}
			}}
		/* Find '+'/'-' that is not a  number sign but an operator. */	
		else if ( *Point == '+' || *Point == '-' ) {
			if ( *(Point - 1) != '*' && *(Point - 1) != '/'
			 && *(Point - 1) != '^' && *(Point - 1) != '(' )
				break ; /* Found! Leave the for loop.*/
		}
	}
	
	if ( Point <= SubString )
		return ADD_SUB_R ( SubString , StringTail ) ;
	else if ( *Point == '+' )
		return ADD_SUB_L (SubString, Point-1) + ADD_SUB_R (Point+1, StringTail) ;
	else /* *Point == '-' */
		return ADD_SUB_L (SubString, Point-1) - ADD_SUB_R (Point+1, StringTail) ;
}

INTN MultiplieDivise (const CHAR16* SubString, const CHAR16* StringTail) {
	const CHAR16 *Point ;
	INTN Chack = 0 ;

	for ( Point = StringTail ; Point > SubString ; Point-- ) {
		/* If there are, skip characters between parenthes pair.  */
		if ( *Point == ')' ) {
			Chack++ ;
			while ( Point > SubString && Chack != 0 ) {
				Point-- ;
				switch ( *Point ) {
					case ')' : Chack++ ; break ;
					case '(' : Chack-- ; break ;
				}
			}}
		else if ( *Point == '*' || *Point == '/' )
			break ; /* Found operator, leave the for loop.*/
	}
	
	if ( Point <= SubString )
		return MUL_DIV_R ( SubString , StringTail ) ;
	else if ( *Point == '*' )
		return MUL_DIV_L (SubString, Point-1) * MUL_DIV_R (Point+1, StringTail) ;
	else /* *Point == '/' */
		return MUL_DIV_L (SubString, Point-1) / MUL_DIV_R (Point+1, StringTail) ;
}

INTN ToThePowerOf (const CHAR16* SubString, const CHAR16* StringTail) {
	const CHAR16 *Point ;
	INTN Chack = 0 ;
	INTN Index, PowBase, PowSum = 1 ;

	for ( Point = SubString ; Point < StringTail ; Point++ ) {
		/* If there are, skip characters between parenthes pair.  */
		if ( *Point == '(' ) {
			Chack++ ;
			while ( Point < StringTail && Chack != 0 ) {
				Point++ ;
				switch ( *Point ) {
					case '(' : Chack++ ; break ;
					case ')' : Chack-- ; break ;
				}
			}}
		else if ( *Point == '^' )
			break ; /* Found operator, leave the for loop.*/
	}
	
	if ( Point >= StringTail )
		return POW_BASE ( SubString , StringTail ) ;
	else { /* *Point == '^' */
		PowBase = POW_BASE ( SubString , StringTail ) ;
		for ( Index = POW_EXP ( Point + 1 , StringTail ) ; Index > 0 ; Index-- ) {
			PowSum = PowSum * PowBase ;
		}
		return PowSum ;
	}
}

INTN PeelParenthes (const CHAR16* SubString, const CHAR16* StringTail) {

	if ( *StringTail == ')' ) {
		if ( *SubString == '-' )
			return -1 * IN_PRNTH ( SubString + 2 , StringTail - 1) ;
		else
			return IN_PRNTH ( SubString + 1 , StringTail - 1) ; }
	else return NO_PRNTH ( SubString , StringTail ) ;
}

INTN GetNumber (const CHAR16* SubString, const CHAR16* StringTail) {
	CHAR16 Tmp ;
	INTN Number;

	Tmp = *( (CHAR16*) StringTail + 1) ;
	*( (CHAR16*) StringTail + 1) = '\0' ;
	
	if ( *SubString == '-' ) {
		SubString += 1 ;
		Number = - Atoi ( SubString ) ;
	} else {
		Number = Atoi ( SubString ) ;
	}

	*( (CHAR16*) StringTail + 1) = Tmp;

	return Number ;
}
