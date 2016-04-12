// execute opcode
switch (opcode) {
case 0x00: // And
	_registers[rd] = _registers[rn] & operand2;
	break;
case 0x01: // And flags
	break;
case 0x02: // Exclusive or
	_registers[rd] = _registers[rn] ^ operand2;
	break;
case 0x03: // Exclusive or flags
	break;
case 0x04: // Sub
	_registers[rd] = _registers[rn] - operand2;
	break;
case 0x05: // Sub flags
	break;
case 0x06: // Reverse subtract
	_registers[rd] = operand2 - _registers[rn];
	break;
case 0x07: // Reverse subtract flags
	break;
case 0x08: // Add
	_registers[rd] = _registers[rn] + operand2;
	break;
case 0x09: // Add flags
	break;
case 0x0A: // Add with carry
	_registers[rd] = _registers[rn] + operand2 + ((_cpsr & CPSR_C_MASK) != 0 ? 1 : 0);
	break;
case 0x0B: // Add with carry flags
	break;
case 0x0C: // Subtract with carry
	_registers[rd] = _registers[rn] - operand2 - ((_cpsr & CPSR_C_MASK) == 0 ? 1 : 0);
	break;
case 0x0D: // Subtract with carry flags
	break;
case 0x0E: // reverse subtract with carry
	_registers[rd] = operand2 - _registers[rn] - ((_cpsr & CPSR_C_MASK) == 0 ? 1 : 0);
	break;
case 0x0F: // Reverse subtract with carry flags
	break;
case 0x10: // Test
	break;
case 0x11: // Test flags
	break;
case 0x12: // Test equivalence
	break;
case 0x13: // Test equivalence flags
	break;
case 0x14: // Compare
	break;
case 0x15: // Compare flags
	break;
case 0x16: // Compare negated
	break;
case 0x17: // Compare negated flags
	break;
case 0x18: // logical OR
	_registers[rd] = _registers[rn] | operand2;
	break;
case 0x19: // Logical Or flags
	break;
case 0x1A: // Move
	_registers[rd] = operand2;
	break;
case 0x1B: // Move flags
	break;
case 0x1C: // Bit clear
	_registers[rd] = _registers[rn] & ~operand2;
	break;
case 0x1D: // Bit clear flags
	break;
case 0x1E: // Move Not
	_registers[rd] = ~operand2;
	break;
case 0x1F: // Move Not flags
	break;
}