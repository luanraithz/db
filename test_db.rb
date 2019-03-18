describe 'database' do
  def clear_db()
    if (File.exists?("test_db.db"))
      File.delete("test_db.db")
    end
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
    clear_db();
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
    clear_db();
    script = (1..1401).map do |i|
      "insert #{i} user #{i} person#{i}@example.com"
    end

    script << ".exit"
    result = run_script(script)
    expect(result[-2]).to eq('db > Error: Table full.')
  end

  it 'allows inserting strings that are the maximum length' do
    clear_db();
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
    clear_db();
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
    clear_db();
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
end