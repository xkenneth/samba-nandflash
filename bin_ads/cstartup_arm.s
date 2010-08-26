;-----------------------------------------------------------------------------
;* This program is free software; you can redistribute it and/or modify
;* it under the terms of the GNU General Public License as published by
;* the Free Software Foundation; either version 2 of the License, or
;* (at your option) any later version.
;-----------------------------------------------------------------------------
;- File source          : cstartup_boot.arm
;- Object               : Generic CStartup
;- Compilation flag     : None
;- Author				: FBr
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
;- Area Definition
;-----------------
;- Must be defined as function to put first in the code as it must be mapped
;- at offset 0 of the flash EBI_CSR0, ie. at address 0 before remap.
;------------------------------------------------------------------------------
                AREA        reset, CODE, READONLY


;------------------------------------------------------------------------------
;- Define the entry point
;------------------------

	EXPORT	__ENTRY
__ENTRY

;-------------------
;- The reset handler
;-------------------
InitReset

;-------------------------------------
;- Setup the stack for supervisor mode
;-------------------------------------
	ldr     sp, = TOP_OF_MEMORY	; Init stack SVC
	stmfd   sp!, {lr}	

;------------------------------------------------------------------------------
;- Branch on C code Main function (with interworking)
;----------------------------------------------------
;- Branch must be performed by an interworking call as either an ARM or Thumb 
;- main C function must be supported. This makes the code not position-
;- independant. A Branch with link would generate errors 
;------------------------------------------------------------------------------
	IMPORT      main

	bl        main
	
;------------------------------------------------------------------------------
;- Jump back
;------------------------------------------------------------------------------
	ldmia      sp!, {r0}^
	bx         r0
	
	
	
            END
