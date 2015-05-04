require 'formula'

class Mastermind < Formula
  version "0.1.3"
  url "https://github.com/lejenome/mastermind/archive/v#{version}.tar.gz"
  head "https://github.com/lejenome/mastermind.git", :using => :git
  homepage "https://github.com/lejenome/mastermind/"
  sha256 ""

  depends_on "cmake" => :build
  depends_on "readline" => :recommended
  depends_on "gettext" => :build
  depends_on "sdl2"
  depends_on "sdl2_ttf"

  def install
    system "cmake", "-DCMAKE_INSTALL_PREFIX=#{prefix}", "-DCMAKE_BUILD_TYPE=Release", "-DCMAKE_BUILD_WITH_INSTALL_RPATH=ON", "-DCMAKE_INSTALL_NAME_DIR=#{HOMEBREW_PREFIX}/lib", "."
#   system "cmake", ".", *std_cmake_args
    system "make", "install"
  end
end
