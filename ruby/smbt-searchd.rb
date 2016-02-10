require "open3"
require "logger"

module SMBT
  # SMBT::Server
  #
  # serv = SMBT::Server.new("./smbt-searchd", "../index")
  # serv.logger("smbt-searchd.log")
  #
  # serv.start do |s|
  #   res = s.search(0.9, "graph-databse-file.dat")
  # end
  #
  class Server
    def initialize(smbt_searchd = nil, indexfile = nil)
      @smbt_searchd = smbt_searchd
      @indexfile    = indexfile
      @status       = nil
      @logger       = Logger.new(STDOUT)
      @logger.level = Logger::INFO
    end
    attr_accessor :smbt_searchd
    attr_accessor :indexfile
    attr_accessor :logger

    def logger(logfile = nil)
      loglevel = @logger.level
      @logger = Logger.new(logfile)
      @logger.level = loglevel
      @logger
    end

    def start
      @logger.info("Start.")
      if block_given?
        @status = connect
        yield self
      else
        @status = connect
        return @status
      end
      stop
    end

    def stop
      @logger.info("Stop.")
      disconnect
      @status = @wait_thr.status
    end

    def restart
      stop
      start
    end

    def connect
      cmd = "#{@smbt_searchd} #{@indexfile}"
      @sdin, @sdout, @sderr, @wait_thr = Open3.popen3(cmd)
      @logger.info("connect pid: #{@wait_thr[:pid]}, status: #{@wait_thr.status}")
      @wait_thr.status
    end

    def disconnect
      @logger.info("disconnect pid: #{@wait_thr[:pid]}, status: #{@wait_thr.status}")
      @sdin.close
      @sdout.close
      @sderr.close
      begin
        Process.kill("USR1", @wait_thr[:pid])
      rescue Errno::ESRCH
        @logger.warn("disconnect pid:#{@wait_thr[:pid]} Errno::ESRCH in disconnect")
      end
    end

    def reconnect
      disconnect
      @status = connect
    end

    def search(threshold, qfname)
      @logger.info("Search search(#{threshold}, \"#{qfname}\")")
      cmd = "SEARCH #{threshold} #{qfname}"
      res = ""
      begin
        @sdin.puts cmd
        while line = @sdout.gets
          res << line
          break if line.chomp == "///"
        end
      rescue Errno::EPIPE
        @logger.warn("Search Errno::EPIPE in search(#{threshold}, \"#{qfname}\")")
        start
        return search(threshold, qfname)
      end
      return SearchResult.new(res)
    end
  end

  # SMBT::SearchResult
  #
  # similarity: 0.8
  # qfname:     ../dat/fingerprints.1.dat
  #
  # query id:0 num:4 860:0.950413 902:0.912698 0:1 9411:0.802817
  # average answers:4
  # average cpu time (sec):0.040823
  # variance:nan
  class SearchResult
    def initialize(str)
      @similarity       = nil
      @results          = []
      @qfname           = nil
      @average_answer   = nil
      @average_cpu_time = nil
      @variance         = nil
      @status           = true
      str.split("\n").each do |line|
        line.chomp!
        case line
        when /^similarity:\s*(\d\S*)/
          @similarity = $1
        when /^qfname:\s*(\S+)/
          @qfname = $1
        when /^query id:(\d+)/
          @results << parse_result(line)
        when /^average answers:\s*(\d\S*)/
          @average_answer = $1
        when /^average cpu time \(sec\):\s*(\d\S*)/
          @average_cpu_time = $1
        when /^variance:(\S+)/
          @variance = $1
        else
        end
      end
      @status = false unless @average_answer or @average_cpu_time or @variance
      self
    end

    private

    # query id:0 num:3 860:0.950413 902:0.912698 0:1
    def parse_result(qline)
      q = qline.split(" ")
      query = q.shift
      query_id = q.shift.split(":")[1].strip.to_i
      num = q.shift.split(":")[1].strip.to_i
      hits = {}
      q.each do |x|
        hid, score = x.split(":")
        hits[hid.to_i] = score.to_f
      end
      {:query_id => query_id, :num => num, :hits => hits}
    end
  end
end


if __FILE__ == $0
  puts "=== SMBT::Server and SMBT::SearchResult ==="
  serv = SMBT::Server.new("../prog/smbt-searchd", "../index")
  serv.logger.progname = "smbt-searchd.rb"
  serv.start
  queries = [{:t => 0.9, :qfname => "../dat/fingerprints.1.dat"},
             {:t => 0.8, :qfname => "../dat/cannot-open-graph-file.dat"},
             {:t => 0.8, :qfname => "../dat/fingerprints.1.dat"},
             {:t => 0.9, :qfname => "../dat/fingerprints.2.dat"}]
  queries.each do |q|
    puts "\n == Start #{q.inspect}"
    res = serv.search(q[:t], q[:qfname])
    p res
    puts " == End\n"
  end

  serv.stop
  p serv

puts "\n\n==== Case: Cannot open graph file. ==="
  serv = SMBT::Server.new
  serv.smbt_searchd = "../prog/smbt-searchd"
  serv.indexfile = "../index"

  serv.start
  p serv
  p res = serv.search(0.8, "../dat/fingrpints.1.dat")
  p serv
  serv.stop
  p serv


  puts "\n\n==== Case: start with block and cannot open graph file. ==="

  serv.start do |s|
    p s
    p res = s.search(0.8, "../dat/fingerprints.1.dat")
    p res = s.search(0.8, "../dat/HOGE.1.dat")
    p res = s.search(0.8, "../dat/fingerprints.1.dat")
  end
  p serv

  puts "\n\n==== Case: index mode 1 ==="
  serv = SMBT::Server.new("../prog/smbt-searchd", "../index.1")
  serv.start do |s|
    p res = s.search(0.8, "../dat/fingerprints.1.dat")
  end

  puts "\n\n==== Case: index mode 2 ==="
  serv = SMBT::Server.new("../prog/smbt-searchd", "../index.2")
  serv.start do |s|
    p res = s.search(0.8, "../dat/fingerprints.1.dat")
  end

  puts "\n\n==== Case: index mode 3 ==="
  serv = SMBT::Server.new("../prog/smbt-searchd", "../index.3")
  serv.start do |s|
    p res = s.search(0.8, "../dat/fingerprints.1.dat")
  end

end
