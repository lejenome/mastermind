class Mastermind < Formula
  version "0.1.2"
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
    system "cmake", "-DCMAKE_INSTALL_PREFIX=#{HOMEBREW_PREFIX}", "-DCMAKE_BUILD_TYPE=Release", "."
#   system "cmake", ".", *std_cmake_args
    system "make", "all", "translations"
    system "make", "install"
  end
end
