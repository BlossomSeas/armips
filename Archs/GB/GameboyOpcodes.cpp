#include "Archs/GB/GameboyOpcodes.h"

const tGameboyOpcode GameboyOpcodes[] = {
	//	Name	Len	Encode	Left param				Right param				LShift	RShift	Flags
	{ L"nop",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
	{ L"ld",	1,	0x40,	GB_PARAM_REG8_MEMHL,	GB_PARAM_REG8_MEMHL,	3,		0,		GB_LOAD_REG8_REG8 },
	{ L"ld",	1,	0x0A,	GB_PARAM_A,				GB_PARAM_MEMBC_MEMDE,	-1,		4,		0 },
	{ L"ld",	1,	0x02,	GB_PARAM_MEMBC_MEMDE,	GB_PARAM_A,				4,		-1,		0 },
	{ L"ld",	2,	0x06,	GB_PARAM_REG8_MEMHL,	GB_PARAM_IMMEDIATE,		3,		-1,		GB_IMMEDIATE_U8 },
	{ L"ld",	3,	0x01,	GB_PARAM_REG16,			GB_PARAM_IMMEDIATE,		4,		-1,		GB_IMMEDIATE_U16 },
	{ L"ld",	3,	0x08,	GB_PARAM_MEMIMMEDIATE,	GB_PARAM_SP,			-1,		-1,		GB_IMMEDIATE_U16 },
//	{ L"ldi",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"ldd",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"push",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"pop",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"add",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"adc",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"sub",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"sbc",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"and",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"xor",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"or",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"cp",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"inc",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"dec",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
	{ L"daa",	1,	0x27,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
	{ L"cpl",	1,	0x2F,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
	{ L"rlca",	1,	0x07,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
	{ L"rla",	1,	0x17,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
	{ L"rrca",	1,	0x0F,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
	{ L"rra",	1,	0x1F,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"rlc",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"rl",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"rrc",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"rr",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"sla",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"swap",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"sra",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"srl",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"bit",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"set",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"res",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
	{ L"ccf",	1,	0x3F,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
	{ L"scf",	1,	0x37,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
	{ L"halt",	1,	0x76,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
	{ L"stop",	2,	0x10,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		GB_STOP },
	{ L"di",	1,	0xF3,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
	{ L"ei",	1,	0xFB,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"jp",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"jr",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"call",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"ret",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"reti",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
//	{ L"rst",	1,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
	{ nullptr,	0,	0x00,	GB_PARAM_NONE,			GB_PARAM_NONE,			-1,		-1,		0 },
};
