IF !DEF(SECOND)
TYPE equs "HRAM"
ELSE
TYPE equs "WRAM0"
ENDC

SECTION UNION "conflicting types", TYPE
	db

	PURGE TYPE
