require 'formula'

class PhpQb < Formula
  homepage 'http://www.php-qb.net/'
  url 'https://php-qb.googlecode.com/files/qb-1.2-source-code.tar.bz2'
  sha1 '8474fa9475c4841d31791cc5abfa4784eb9c93e1'

  def install
    system "phpize"
    system "./configure"
    system "make"
    system "sudo", "make", "install"
  end

  test do
    system "echo \"<?php exit(extension_loaded('qb') || dl('qb.so') ? 0 : -1); ?>\" | php"
  end
end
