require "rspec/expectations"

describe 'database' do
  def clear_db()
    if (File.exists?("test_db.db"))
      File.delete("test_db.db")
    end
  end

  before(:each) do 
    clear_db()
  end
  def run_script(commands)
    raw_output = nil
    IO.popen("./db test_db.db", "r+") do |pipe|
      commands.each do |command|
        pipe.puts command
      end

      pipe.close_write

      # Read entire output
      raw_output = pipe.gets(nil)
    end
    raw_output.split("\n")
  end

  it 'inserts and retreives a row' do
    result = run_script([
      "insert 1 user1 person1@example.com",
      "select",
      ".exit",
    ])

    expect(result).to match_array([
      "db > Executed.",
      "db > (1, user1, person1@example.com)",
      "Executed.",
      "db > ",
    ])
  end

  it 'prints error message when table is full' do
    script = (1..1401).map do |i|
      "insert #{i} user #{i} person#{i}@example.com"
    end

    script << ".exit"
    result = run_script(script)
    expect(result[-2]).to eq('db > Error: Table full.')
  end

  it 'allows inserting strings that are the maximum length' do
    long_username = "a"*32
    long_email = "a"*255
    script = [
      "insert 1 #{long_username} #{long_email}",
      "select",
      ".exit",
    ]

    result = run_script(script)
    expect(result).to match_array([
      "Executed.",
      "db > (1, #{long_username}, #{long_email})",
      "db > Executed.",
      "db > ",
    ])
  end

  it 'doesn\'t allow inserting email with more than the maximum length' do
    long_email = "a"*300
    script = [
      "insert 1 a #{long_email}",
      "select",
      ".exit"
    ]
    result = run_script(script)

    expect(result).to match_array([
      "db > One of the fields is too large.",
      "db > Executed.",
      "db > "
    ])
  end

  it 'keeps data after closing the programm' do 
    result1 = run_script([
      "insert 1 user1 user1@gmail.com",
      ".exit"
    ])

    expect(result1).to match_array([
      "db > Executed.",
      "db > "
    ])

    result2 = run_script([
      "select",
      ".exit"
    ])

    expect(result2).to match_array([
      "db > (1, user1, user1@gmail.com)",
      "Executed.",
      "db > ",
    ])
  end

  it 'allows printing out the structure of a one-node btree' do
    script = [3, 1, 2].map do |i|
      "insert #{i} user#{i} person#{i}@example.com"
    end
    script << ".btree"
    script << ".exit"

    result = run_script(script)

    expect(result).to match_array([
      "db > Executed.",
      "db > Executed.",
      "db > Executed.",
      "db > Tree: ",
      "Leaf (size 3)",
      " - 0 : 1",
      " - 1 : 2",
      " - 2 : 3",
      "db > ",
    ])
  end

  it 'prints constants' do
    script = [
      ".constants",
      ".exit"
    ]

    result = run_script(script)

    expect(result).to match_array([
      "db > Constants: ",
      "ROW_SIZE: 293",
      "COMMON_NODE_HEADER_SIZE: 6",
      "LEAF_NODE_HEADER_SIZE: 10",
      "LEAF_NODE_CELL_SIZE: 297",
      "LEAF_NODE_SPACE_FOR_CELLS: 4086",
      "LEAF_NODE_MAX_CELLS: 13",
      "db > "
    ])

  end

  it 'prints an error message if there is a duplicate id' do
    script = [
      "insert 1 user1 person1@example.com",
      "insert 1 user1 person1@example.com",
      "select",
      ".exit"
    ]

    result = run_script(script)

    expect(result).to match_array([
      "db > Executed.",
      "db > Error: Duplicate key.",
      "db > (1, user1, person1@example.com)",
      "Executed.",
      "db > "
    ])

  end
end
