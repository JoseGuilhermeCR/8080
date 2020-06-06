use std::fs;
use std::collections::HashMap;

// TODO: Panic with error messages for instructions as well.

struct MissingLabel {
    label: String,
    position: u16,
    line_number: usize,
}

impl MissingLabel {
    fn new(label: String, position: u16, line_number: usize) -> MissingLabel {
        MissingLabel {
            label,
            position,
            line_number,
        }
    }
}

struct AssemblerData {
    machine_code: Vec<u8>,
    label_address: HashMap<String, u16>,
    origin_point: u16,
    missing_labels: Vec<MissingLabel>,
    current_line: usize,
}

impl AssemblerData {
    fn new() -> AssemblerData {
        AssemblerData {
            machine_code: vec![],
            label_address: HashMap::new(),
            origin_point: 0,
            missing_labels: vec![],
            current_line: 0,
        }
    }
}
#[derive(Clone, Debug)]
enum InstructionType {
    NoOperand,
    Reg,
    RegWord,
    RegByte,
    RegReg,
    Byte,
    Word,
    Numeric,
}

#[derive(Clone, Debug)]
struct Instruction {
    code: u8,
}

impl Instruction {
    fn new(code: u8) -> Instruction {
        Instruction {
            code,
        }
    }
}

fn assemble(code: String, mnemonic_type: HashMap<&str, InstructionType>, instruction_code: HashMap<&str, Instruction>) -> AssemblerData {
    let mut data = AssemblerData::new();
    
    for line in code.lines() {
        data.current_line += 1;

        // Remove possible comment.
        let line = line.split(";").next().unwrap();
        
        if !line.is_empty() {
            // Process possible label
            let line = process_label(line, &mut data);
            
            if !line.is_empty() {
                match line.trim().chars().next().unwrap() {
                    '.' => process_assembler_command(line, &mut data),
                    _ => process_assembly_line(line, &mut data, &mnemonic_type, &instruction_code),
                }
            }
        }
    }
    
    // Go through all missing labels and fix them... If they appeared later on in the assembly code...
    for missing in &data.missing_labels {
        match data.label_address.get(missing.label.as_str()) {
            None => panic!("Error at line {} / Missing label: {}", missing.line_number, missing.label),
            Some(address) => {
                let address = (address + data.origin_point).to_le_bytes();
                data.machine_code[missing.position as usize] = address[0];
                data.machine_code[(missing.position + 1) as usize] = address[1];
            }
        }
    }
    
    data
}

fn process_label<'a>(line: &'a str, data: &mut AssemblerData) -> &'a str {
    match line.find(":") {
        None => line.trim(),
        Some(index) => {
            let result = data.label_address.insert(line[..index].to_string(), data.machine_code.len() as u16);
            match result {
                None => (),
                Some(_) => println!("Warning: Label \"{}\" was defined before. It's value will be that of the last position it appeared", line[..index].to_string()),
            }
            line[index+1..].trim()
        },
    }
}

fn process_assembly_line(line: &str, data: &mut AssemblerData, mnemonic_type: &HashMap<&str, InstructionType>, instruction_code: &HashMap<&str, Instruction>) {
    println!("{}", line);
    // Converts line to lowercase.
    let line = line.to_ascii_lowercase();
    
    let mnemonic = line.split_whitespace().next().unwrap();
    
    match mnemonic_type.get(mnemonic).unwrap() {
        InstructionType::NoOperand => {
            let instr = instruction_code.get(mnemonic).unwrap();
            add_machine_code_instr(instr, None, None, data);
        },
        InstructionType::Reg => {
            let instr = instruction_code.get(line.split_whitespace().collect::<String>().as_str()).unwrap(); 
            add_machine_code_instr(instr, None, None, data);
        },
        InstructionType::RegByte => {
            let mut iter = line.split(",");
            
            let instr = instruction_code.get(iter.next().unwrap().trim().split_whitespace().collect::<String>().as_str()).unwrap(); 
            let byte = byte_from_line(iter.next().unwrap().trim()).unwrap();
            add_machine_code_instr(instr, Some(byte), None, data);
        },
        InstructionType::RegWord => {
            let mut iter = line.split(",");
            let instr = iter.next().unwrap().trim().split_whitespace().collect::<String>();
            let word = process_word_or_label(iter.next().unwrap().trim(), data);
            
            let instr = instruction_code.get(instr.as_str()).unwrap();
            add_machine_code_instr(instr, Some(word[0]), Some(word[1]), data);
        },
        InstructionType::RegReg => {
            let instr = instruction_code.get(line.split(",").collect::<String>().split_whitespace().collect::<String>().as_str()).unwrap(); 
            add_machine_code_instr(instr, None, None, data);
        },
        InstructionType::Byte => {
            let mut iter = line.split_whitespace();
            let instr = instruction_code.get(iter.next().unwrap()).unwrap();
            let byte = byte_from_line(iter.next().unwrap().trim()).unwrap();
            
            add_machine_code_instr(instr, Some(byte), None, data);
        },
        InstructionType::Word => {
            let mut iter = line.split_whitespace();
            let instr = iter.next().unwrap().trim();
            let word = process_word_or_label(iter.next().unwrap().trim(), data);
            
            let instr = instruction_code.get(instr).unwrap();
            add_machine_code_instr(instr, Some(word[0]), Some(word[1]), data);
        },
        InstructionType::Numeric => {
            let instr = instruction_code.get(line.split_whitespace().collect::<String>().as_str()).unwrap();
            add_machine_code_instr(instr, None, None, data);
        },
    }
}

fn process_word_or_label(operand: &str, data: &mut AssemblerData) -> [u8; 2] {
    match word_from_line(operand) {
        Some(word) => word,
        None => {
            // Check if label is already in the map:
            match data.label_address.get(operand) {
                Some(word) => *word + data.origin_point,
                None => {
                    data.missing_labels.push(MissingLabel::new(operand.to_string(), (data.machine_code.len() + 1) as u16, data.current_line));
                    0x0000
                }
            }
        },
    }.to_le_bytes()
}

fn add_machine_code_instr(instr: &Instruction, lower_byte: Option<u8>, higher_byte: Option<u8>, data: &mut AssemblerData) {
    data.machine_code.push(instr.code);
    add_machine_code_operand(lower_byte, data);
    add_machine_code_operand(higher_byte, data);
}

fn add_machine_code_operand(byte: Option<u8>, data: &mut AssemblerData) {
    match byte {
        Some(byte) => data.machine_code.push(byte),
        None => (),
    }
}

fn process_assembler_command(line: &str, data: &mut AssemblerData) {
    let split_command= line.split_whitespace().collect::<Vec<&str>>();
    
    // There are missing arguments.
    if split_command.len() == 1 {
        panic!("Error at line {}: {} / No arguments for command: {}", data.current_line, line, split_command[0]);
    }

    match split_command[0] {
        ".org" => {
            match word_from_line(split_command[1]) {
                None => panic!("Error at line {}: {} / Invalid number: {}", data.current_line, line, split_command[1]),
                Some(word) => data.origin_point = word,
            }
        },
        ".db" => {
            match byte_from_line(split_command[1]) {
                None => panic!("Error at line {}: {} / Invalid number: {}", data.current_line, line, split_command[1]),
                Some(byte) => data.machine_code.push(byte)
            }
        },
        ".asciiz" => {
            // Find the start and end of the ascii string.
            let start = line.find("\"");
            let end = line.rfind("\"");

            // No " found.
            if start == None || end == None {
                panic!("Error at line {}: {} / Invalid String", data.current_line, line);
            }

            let start = start.unwrap();
            let end = end.unwrap();

            // Found ", but only one.
            if start == end {
                panic!("Error at line {}: {} / Invalid String", data.current_line, line);
            }

            for c in line[start+1..end].chars() {
                data.machine_code.push(c as u8);
            }
            data.machine_code.push(0); // null terminator.
        },
        _ => (),
    }
}

fn word_from_line(number: &str) -> Option<u16> {
    let (radix, number) = if number.contains("0x") {
        (16, number[2..].to_string())
    } else if number.contains("h") {
        (16, number[..number.len()-1].to_string())
    } else {
        (10, number.to_string())
    };
    
    match u16::from_str_radix(&number, radix) {
        Ok(result) => Some(result),
        Err(_) => None,
    }
}

fn byte_from_line(number: &str) -> Option<u8> {
    let (radix, number) = if number.contains("0x") {
        (16, number[2..].to_string())
    } else if number.contains("h") {
        (16, number[..number.len()-1].to_string())
    } else {
        (10, number.to_string())
    };
    
    match u8::from_str_radix(&number, radix) {
        Err(_) => None,
        Ok(number) => Some(number),
    }
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    
    if args.len()  < 2 {
        println!("How to use:
        \rassembler file.asm --output file.hex");
        std::process::exit(1);
    } else {
        let input = &args[1];
        let mut output = "file.hex";
        
        for arg in args.iter().enumerate() {
            if arg.1 == "--output" && arg.0 + 1 < args.len() {
                output = &args[arg.0 + 1];
            }
        }
        
        let assembly = fs::read_to_string(input).unwrap();
        let machine_code = assemble(assembly, create_mnemonic_type_map(), create_instruction_code_map()).machine_code;
        fs::write(output, machine_code);
    }
}

fn create_mnemonic_type_map() -> HashMap<&'static str, InstructionType> {
    [
    ("nop", InstructionType::NoOperand),
    ("lxi", InstructionType::RegWord),
    ("stax", InstructionType::Reg),
    ("inx", InstructionType::Reg),
    ("inr", InstructionType::Reg),
    ("dcr", InstructionType::Reg),
    ("mvi", InstructionType::RegByte),
    ("rlc", InstructionType::NoOperand),
    ("dad", InstructionType::Reg),
    ("ldax", InstructionType::Reg),
    ("dcx", InstructionType::Reg),
    ("rrc", InstructionType::NoOperand),
    ("ral", InstructionType::NoOperand),
    ("rar", InstructionType::NoOperand),
    ("shld", InstructionType::Word),
    ("daa", InstructionType::NoOperand),
    ("lhld", InstructionType::Word),
    ("cma", InstructionType::NoOperand),
    ("sta", InstructionType::Word),
    ("stc", InstructionType::NoOperand),
    ("lda", InstructionType::Word),
    ("cmc", InstructionType::NoOperand),
    ("mov", InstructionType::RegReg),
    ("hlt", InstructionType::NoOperand),
    ("add", InstructionType::Reg),
    ("adc", InstructionType::Reg),
    ("sub", InstructionType::Reg),
    ("sbb", InstructionType::Reg),
    ("ana", InstructionType::Reg),
    ("xra", InstructionType::Reg),
    ("ora", InstructionType::Reg),
    ("cmp", InstructionType::Reg),
    ("rnz", InstructionType::NoOperand),
    ("pop", InstructionType::Reg),
    ("jnz", InstructionType::Word),
    ("jmp", InstructionType::Word),
    ("cnz", InstructionType::Word),
    ("push", InstructionType::Reg),
    ("adi", InstructionType::Byte),
    ("rst", InstructionType::Numeric),
    ("rz", InstructionType::NoOperand),
    ("ret", InstructionType::NoOperand),
    ("jz", InstructionType::Word),
    ("cz", InstructionType::Word),
    ("call", InstructionType::Word),
    ("aci", InstructionType::Byte),
    ("rnc", InstructionType::NoOperand),
    ("jnc", InstructionType::Word),
    ("out", InstructionType::Byte),
    ("cnc", InstructionType::Word),
    ("sui", InstructionType::Byte),
    ("rc", InstructionType::NoOperand),
    ("jc", InstructionType::Word),
    ("in", InstructionType::Byte),
    ("cc", InstructionType::Word),
    ("sbi", InstructionType::Byte),
    ("rpo", InstructionType::NoOperand),
    ("jpo", InstructionType::Word),
    ("xthl", InstructionType::NoOperand),
    ("cpo", InstructionType::Word),
    ("ani", InstructionType::Byte),
    ("rpe", InstructionType::NoOperand),
    ("pchl", InstructionType::NoOperand),
    ("jpe", InstructionType::Word),
    ("xchg", InstructionType::NoOperand),
    ("cpe", InstructionType::Word),
    ("xri", InstructionType::Byte),
    ("rp", InstructionType::NoOperand),
    ("jp", InstructionType::Word),
    ("di", InstructionType::NoOperand),
    ("cp", InstructionType::Word),
    ("ori", InstructionType::Byte),
    ("rm", InstructionType::NoOperand),
    ("sphl", InstructionType::NoOperand),
    ("jm", InstructionType::Word),
    ("ei", InstructionType::NoOperand),
    ("cm", InstructionType::Word),
    ("cpi", InstructionType::Byte),
    ].iter().cloned().collect()
}

fn create_instruction_code_map() -> HashMap<&'static str, Instruction> {
    [
    ("nop", Instruction::new(0x00)),
    ("lxib", Instruction::new(0x01)),
    ("staxb", Instruction::new(0x02)),
    ("inxb", Instruction::new(0x03)),
    ("inrb", Instruction::new(0x04)),
    ("dcrb", Instruction::new(0x05)),
    ("mvib", Instruction::new(0x06)),
    ("rlc", Instruction::new(0x07)),
    ("dadb", Instruction::new(0x09)),
    ("ldaxb", Instruction::new(0x0A)),
    ("dcxb", Instruction::new(0x0B)),
    ("inrc", Instruction::new(0x0C)),
    ("dcrc", Instruction::new(0x0D)),
    ("mvic", Instruction::new(0x0E)),
    ("rrc", Instruction::new(0x0F)),
    ("lxid", Instruction::new(0x11)),
    ("staxd", Instruction::new(0x12)),
    ("inxd", Instruction::new(0x13)),
    ("inrd", Instruction::new(0x14)),
    ("dcrd", Instruction::new(0x15)),
    ("mvid", Instruction::new(0x16)),
    ("ral", Instruction::new(0x17)),
    ("dadd", Instruction::new(0x19)),
    ("ldaxd", Instruction::new(0x1A)),
    ("dcxd", Instruction::new(0x1B)),
    ("inre", Instruction::new(0x1C)),
    ("dcre", Instruction::new(0x1D)),
    ("mvie", Instruction::new(0x1E)),
    ("rar", Instruction::new(0x1F)),
    ("lxih", Instruction::new(0x21)),
    ("shld", Instruction::new(0x22)),
    ("inxh", Instruction::new(0x23)),
    ("inrh", Instruction::new(0x24)),
    ("dcrh", Instruction::new(0x25)),
    ("mvih", Instruction::new(0x26)),
    ("daa", Instruction::new(0x27)),
    ("dadh", Instruction::new(0x29)),
    ("lhld", Instruction::new(0x2A)),
    ("dcxh", Instruction::new(0x2B)),
    ("inrl", Instruction::new(0x2C)),
    ("dcrl", Instruction::new(0x2D)),
    ("mvil", Instruction::new(0x2E)),
    ("cma", Instruction::new(0x2F)),
    ("lxisp", Instruction::new(0x31)),
    ("sta", Instruction::new(0x32)),
    ("inxsp", Instruction::new(0x33)),
    ("inrm", Instruction::new(0x34)),
    ("dcrm", Instruction::new(0x35)),
    ("mvim", Instruction::new(0x36)),
    ("stc", Instruction::new(0x37)),
    ("dadsp", Instruction::new(0x39)),
    ("lda", Instruction::new(0x3A)),
    ("dcxsp", Instruction::new(0x3B)),
    ("inra", Instruction::new(0x3C)),
    ("dcra", Instruction::new(0x3D)),
    ("mvia", Instruction::new(0x3E)),
    ("cmc", Instruction::new(0x3F)),
    ("movbb", Instruction::new(0x40)),
    ("movbc", Instruction::new(0x41)),
    ("movbd", Instruction::new(0x42)),
    ("movbe", Instruction::new(0x43)),
    ("movbh", Instruction::new(0x44)),
    ("movbl", Instruction::new(0x45)),
    ("movbm", Instruction::new(0x46)),
    ("movba", Instruction::new(0x47)),
    ("movcb", Instruction::new(0x48)),
    ("movcc", Instruction::new(0x49)),
    ("movcd", Instruction::new(0x4A)),
    ("movch", Instruction::new(0x4B)),
    ("movch", Instruction::new(0x4C)),
    ("movcl", Instruction::new(0x4D)),
    ("movcm", Instruction::new(0x4E)),
    ("movca", Instruction::new(0x4F)),
    ("movdb", Instruction::new(0x50)),
    ("movdc", Instruction::new(0x51)),
    ("movdd", Instruction::new(0x52)),
    ("movde", Instruction::new(0x53)),
    ("movdh", Instruction::new(0x54)),
    ("movdl", Instruction::new(0x55)),
    ("movdm", Instruction::new(0x56)),
    ("movda", Instruction::new(0x57)),
    ("moveb", Instruction::new(0x58)),
    ("movec", Instruction::new(0x59)),
    ("moved", Instruction::new(0x5A)),
    ("moveh", Instruction::new(0x5B)),
    ("moveh", Instruction::new(0x5C)),
    ("movel", Instruction::new(0x5D)),
    ("movem", Instruction::new(0x5E)),
    ("movea", Instruction::new(0x5F)),
    ("movhb", Instruction::new(0x60)),
    ("movhc", Instruction::new(0x61)),
    ("movhd", Instruction::new(0x62)),
    ("movhe", Instruction::new(0x63)),
    ("movhh", Instruction::new(0x64)),
    ("movhl", Instruction::new(0x65)),
    ("movhm", Instruction::new(0x66)),
    ("movha", Instruction::new(0x67)),
    ("movlb", Instruction::new(0x68)),
    ("movlc", Instruction::new(0x69)),
    ("movld", Instruction::new(0x6A)),
    ("movlh", Instruction::new(0x6B)),
    ("movlh", Instruction::new(0x6C)),
    ("movll", Instruction::new(0x6D)),
    ("movlm", Instruction::new(0x6E)),
    ("movla", Instruction::new(0x6F)),
    ("movmb", Instruction::new(0x70)),
    ("movmc", Instruction::new(0x71)),
    ("movmd", Instruction::new(0x72)),
    ("movme", Instruction::new(0x73)),
    ("movmh", Instruction::new(0x74)),
    ("movml", Instruction::new(0x75)),
    ("hlt", Instruction::new(0x76)),
    ("movma", Instruction::new(0x77)),
    ("movab", Instruction::new(0x78)),
    ("movac", Instruction::new(0x79)),
    ("movad", Instruction::new(0x7A)),
    ("movah", Instruction::new(0x7B)),
    ("movah", Instruction::new(0x7C)),
    ("moval", Instruction::new(0x7D)),
    ("movam", Instruction::new(0x7E)),
    ("movaa", Instruction::new(0x7F)),
    ("addb", Instruction::new(0x80)),
    ("addc", Instruction::new(0x81)),
    ("addd", Instruction::new(0x82)),
    ("adde", Instruction::new(0x83)),
    ("addh", Instruction::new(0x84)),
    ("addl", Instruction::new(0x85)),
    ("addm", Instruction::new(0x86)),
    ("adda", Instruction::new(0x87)),
    ("adcb", Instruction::new(0x88)),
    ("adcc", Instruction::new(0x89)),
    ("adcd", Instruction::new(0x8A)),
    ("adce", Instruction::new(0x8B)),
    ("adch", Instruction::new(0x8C)),
    ("adcl", Instruction::new(0x8D)),
    ("adcm", Instruction::new(0x8E)),
    ("adca", Instruction::new(0x8F)),
    ("subb", Instruction::new(0x90)),
    ("subc", Instruction::new(0x91)),
    ("subd", Instruction::new(0x92)),
    ("sube", Instruction::new(0x93)),
    ("subh", Instruction::new(0x94)),
    ("subl", Instruction::new(0x95)),
    ("subm", Instruction::new(0x96)),
    ("suba", Instruction::new(0x97)),
    ("sbbb", Instruction::new(0x98)),
    ("sbbc", Instruction::new(0x99)),
    ("sbbd", Instruction::new(0x9A)),
    ("sbbe", Instruction::new(0x9B)),
    ("sbbh", Instruction::new(0x9C)),
    ("sbbl", Instruction::new(0x9D)),
    ("sbbm", Instruction::new(0x9E)),
    ("sbba", Instruction::new(0x9F)),
    ("anab", Instruction::new(0xA0)),
    ("anac", Instruction::new(0xA1)),
    ("anad", Instruction::new(0xA2)),
    ("anae", Instruction::new(0xA3)),
    ("anah", Instruction::new(0xA4)),
    ("anal", Instruction::new(0xA5)),
    ("anam", Instruction::new(0xA6)),
    ("anaa", Instruction::new(0xA7)),
    ("xrab", Instruction::new(0xA8)),
    ("xrac", Instruction::new(0xA9)),
    ("xrad", Instruction::new(0xAA)),
    ("xrae", Instruction::new(0xAB)),
    ("xrah", Instruction::new(0xAC)),
    ("xral", Instruction::new(0xAD)),
    ("xram", Instruction::new(0xAE)),
    ("xraa", Instruction::new(0xAF)),
    ("orab", Instruction::new(0xB0)),
    ("orac", Instruction::new(0xB1)),
    ("orad", Instruction::new(0xB2)),
    ("orae", Instruction::new(0xB3)),
    ("orah", Instruction::new(0xB4)),
    ("oral", Instruction::new(0xB5)),
    ("oram", Instruction::new(0xB6)),
    ("oraa", Instruction::new(0xB7)),
    ("cmpb", Instruction::new(0xB8)),
    ("cmpc", Instruction::new(0xB9)),
    ("cmpd", Instruction::new(0xBA)),
    ("cmpe", Instruction::new(0xBB)),
    ("cmph", Instruction::new(0xBC)),
    ("cmpl", Instruction::new(0xBD)),
    ("cmpm", Instruction::new(0xBE)),
    ("cmpa", Instruction::new(0xBF)),
    ("rnz", Instruction::new(0xC0)),
    ("popb", Instruction::new(0xC1)),
    ("jnz", Instruction::new(0xC2)),
    ("jmp", Instruction::new(0xC3)),
    ("cnz", Instruction::new(0xC4)),
    ("pushb", Instruction::new(0xC5)),
    ("adi", Instruction::new(0xC6)),
    ("rst0", Instruction::new(0xC7)),
    ("rz", Instruction::new(0xC8)),
    ("ret", Instruction::new(0xC9)),
    ("jz", Instruction::new(0xCA)),
    ("cz", Instruction::new(0xCC)),
    ("call", Instruction::new(0xCD)),
    ("aci", Instruction::new(0xCE)),
    ("rst1", Instruction::new(0xCF)),
    ("rnc", Instruction::new(0xD0)),
    ("popd", Instruction::new(0xD1)),
    ("jnc", Instruction::new(0xD2)),
    ("out", Instruction::new(0xD3)),
    ("cnc", Instruction::new(0xD4)),
    ("pushd", Instruction::new(0xD5)),
    ("sui", Instruction::new(0xD6)),
    ("rst2", Instruction::new(0xD7)),
    ("rc", Instruction::new(0xD8)),
    ("jc", Instruction::new(0xDA)),
    ("in", Instruction::new(0xDB)),
    ("cc", Instruction::new(0xDC)),
    ("sbi", Instruction::new(0xDE)),
    ("rst3", Instruction::new(0xDF)),
    ("rpo", Instruction::new(0xE0)),
    ("poph", Instruction::new(0xE1)),
    ("jpo", Instruction::new(0xE2)),
    ("xthl", Instruction::new(0xE3)),
    ("cpo", Instruction::new(0xE4)),
    ("pushh", Instruction::new(0xE5)),
    ("ani", Instruction::new(0xE6)),
    ("rst4", Instruction::new(0xE7)),
    ("rpe", Instruction::new(0xE8)),
    ("pchl", Instruction::new(0xE9)),
    ("jpe", Instruction::new(0xEA)),
    ("xchg", Instruction::new(0xEB)),
    ("cpe", Instruction::new(0xEC)),
    ("xri", Instruction::new(0xEE)),
    ("rst5", Instruction::new(0xEF)),
    ("rp", Instruction::new(0xF0)),
    ("poppsw", Instruction::new(0xF1)),
    ("jp", Instruction::new(0xF2)),
    ("di", Instruction::new(0xF3)),
    ("cp", Instruction::new(0xF4)),
    ("pushpsw", Instruction::new(0xF5)),
    ("ori", Instruction::new(0xF6)),
    ("rst6", Instruction::new(0xF7)),
    ("rm", Instruction::new(0xF8)),
    ("sphl", Instruction::new(0xF9)),
    ("jm", Instruction::new(0xFA)),
    ("ei", Instruction::new(0xFB)),
    ("cm", Instruction::new(0xFC)),
    ("cpi", Instruction::new(0xFE)),
    ("rst7", Instruction::new(0xFF)),
    ].iter().cloned().collect()
}
