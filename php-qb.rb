require 'formula'

class PhpQb < Formula
  homepage 'http://www.php-qb.net/'
  url 'https://php-qb.googlecode.com/files/qb-1.3-source-code.tar.bz2'
  sha1 'a9dda48aaae614835fe8d8a769dd8c612ad54232'

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
  
  fails_with :clang do
    build 425
    cause "Compiling with clang is incredibly slow"
  end
end
