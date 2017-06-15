#ifndef OPCODE_PRINTER_H
#define OPCODE_PRINTER_H
#include "opcodes.h"

#include <ostream>

class OpCodePrinter
{
public:
	OpCodePrinter(std::ostream& stream)
		: m_stream(stream)
	{}

	void print(OpCodes code);

private:
	std::ostream& m_stream;
};


#endif // OPCODE_PRINTER_H