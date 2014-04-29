require 'formula'

class PhpQb < Formula
  homepage 'http://www.php-qb.net/'
  url 'https://bitbucket.org/chung_leong/qb/downloads/qb-2.3-source-code.tar.bz2'
  sha1 'edf7226bec15e48e676a6dccc5da846139be6139'

  depends_on 'autoconf' => :build
  
  def install
    system "phpize"
    system "./configure"
    system "make"
    system "cp", "modules/qb.so", "#{prefix}/qb.so"
    system "cp", "qb.ini", "#{prefix}/qb.ini"    
  end

  test do
    system "echo \"<?php exit(extension_loaded('qb') || dl('qb.so') ? 0 : -1); ?>\" | php"
  end
end
