import subprocess
from subprocess import Popen
from typing import List


def cmd_to_inst(cmd:List[str], handle_last:bool = True) -> str:
    if handle_last == True:
        inst = '\n'.join(cmd) + '\n'
    else:
        inst = '\n'.join(cmd)
    return inst

def inst_to_cmd(inst:str) -> List[str]:
    cmd = inst.split("\n")
    return cmd

def running(program:str, cmd:List[str]) -> List[str]:
    p = subprocess.Popen(args=[program, "test.db"],  
    stdin=subprocess.PIPE, 
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True)
    out, err = p.communicate(cmd_to_inst(cmd))
    p.kill()
    return inst_to_cmd(out)


def test_insert(program):
    cmd = [
        "insert 1 user1 person1@example.com", 
        "select", 
        ".exit"
    ]
    expect = [
        # Q:不理解输出的数据为什么会带db> 前缀
        "db> Executed.",
        "db> (1, user1, person1@example.com)",
        "Executed.",
        "db> "
    ]

    output = running(program, cmd)
    assert output == expect


'''
此用例在Part 8章节时，在插入第14行的时候就已经满了
由于校验方式的原因，导致该用例依旧能够通过
'''
def test_table_full(program):
    s = "insert {n} user{n} person{n}@example.com"
    cmd = [s.format(n=i+1) for i in range (0, 1401)]
    cmd.append(".exit")
    output = running(program, cmd)
    expect = "db> Error: Table full."
    assert output[-2] == expect


def test_insert_string_maximum_length(program):
    long_username = 32 * "a"
    long_email = 255 * "a"
    
    cmd = [
        f"insert 1 {long_username} {long_email}",
        "select",
        ".exit"
    ]
    output = running(program, cmd)

    expect = [
        "db> Executed.",
        f"db> (1, {long_username}, {long_email})",
        "Executed.",
        "db> "
    ]
    assert output == expect


def test_print_error_massage_when_string_too_long(program):
    long_username = 33 * "a"
    long_email = 256 * "a"
    cmd = [
        f"insert 1 {long_username} {long_email}",
        "select",
        ".exit"
    ]
    expect = [
        "db> String is too long.",
        "db> Executed.",
        "db> "
    ]
    output = running(program, cmd)
    assert output == expect


def test_insert_nagative_id(program):
    assert running(program,  [
        "insert -1 cstack foo@bar.com",
        "select",
        ".exit"
    ]) == [
        "db> ID must be positive.",
        "db> Executed.",
        "db> "
    ]


def test_persist_data(program):
    # popen.communicate每个生成只能调用一次，而不能反复调用，可以考虑使用pexpect
    assert running(program, [
        "insert 1 user1 person1@example.com",
        "insert 2 user2 person2@example.com",
        ".exit"
    ]) == [
        "db> Executed.",
        "db> Executed.",
        "db> "
    ]

    assert running(program, [
        "select",
        ".exit"
    ]) == [
        "db> (1, user1, person1@example.com)",
        "(2, user2, person2@example.com)",
        "Executed.",
        "db> "
    ]


def test_print_constants(program):
    assert running(program, [
        ".constants",
        ".exit"
    ]) == [
        "db> Constants:",
        "ROW_SIZE: 293",
        "COMMON_NODE_HEADER_SIZE: 6",
        "LEAF_NODE_HEADER_SIZE: 10",
        "LEAF_NODE_CELL_SIZE: 297",
        "LEAF_NODE_CELL_SPACE_FOR_CELLS: 4086",
        "LEAF_NODE_MAX_CELLS: 13",
        "db> "
    ]


def test_print_btree(program):
    cmd = []
    for i in [3, 1, 2]:
        cmd.append(f"insert {i} user{i} person{i}@example.com")
    cmd.append(".btree")
    cmd.append(".exit")

    expect = [
        "db> Executed.",
        "db> Executed.",
        "db> Executed.",
        "db> Tree:",
        "leaf (size 3)",
        "  - 0 : 1",
        "  - 1 : 2",
        "  - 2 : 3",
        "db> "
    ]
    output = running(program, cmd)
    assert output == expect


def test_duplicate_key(program):
    cmd = [
        "insert 1 user1 person1@example.com",
        "insert 1 user1 person1@example.com",
        "select",
        ".exit"
    ]
    expect = [
        "db> Executed.",
        "db> Error: Duplicate key.",
        "db> (1, user1, person1@example.com)",
        "Executed.",
        "db> "
    ]
    output = running(program, cmd)
    assert output == expect