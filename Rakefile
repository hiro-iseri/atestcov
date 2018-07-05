require 'fileutils'
require 'rake/clean'

CPP_COMPILER = "g++"
CPP_COMPILER_FLAGS = "-std=c++11"
INCLUDE_DIR = "include"
SOURCES_DIR = "src"
RELEASE_DIR = "release"
SOURCES = FileList["#{SOURCES_DIR}/*.cpp"]
OBJECTS = SOURCES.ext('o')
TARGET_NAME = "atestcov"

CLEAN.include(["#{SOURCES_DIR}/*.o", "*.exe"])

if /windows/i =~ ENV['OS'] then
    CPP_LD_FLAGS = "-static-libgcc -static-libstdc++"
    TARGET = "#{TARGET_NAME}.exe"
    ENV_NAME = "win"
else
    CPP_LD_FLAGS = ""
    TARGET = "#{TARGET_NAME}"
    ENV_NAME = "macosx"
end

task :default => :build
task :all => [:build]
task :rebuild => ["clean", :build]


desc "build target"
task :build => OBJECTS do |t|
  sh "#{CPP_COMPILER} -o #{TARGET} #{t.prerequisites.join(' ')} #{CPP_LD_FLAGS}"
end

rule '.o' => '.cpp' do |t|
  sh "#{CPP_COMPILER} -I #{INCLUDE_DIR} -c #{t.source} -o #{t.name} #{CPP_COMPILER_FLAGS}"
end

desc "release target"
task :release do
    File.open("#{INCLUDE_DIR}/version.h") do |file|
        version = ""
        file.each_line do |line|
            md = line.match(/[.0-9]+/)
            if md then
                version = md[0]
                break
            end
        end

        next if version.empty?
       
        folder_name = "#{TARGET_NAME}" + "_" + ENV_NAME + "_" + version
        FileUtils.rm_rf("#{RELEASE_DIR}/#{folder_name}")
        FileUtils.cp_r("#{RELEASE_DIR}/atestcov_env_version", "#{RELEASE_DIR}/#{folder_name}")
        FileUtils.mv(TARGET, "#{RELEASE_DIR}/#{folder_name}/#{TARGET}")
    end
end