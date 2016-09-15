Progetto Laboratorio di Ingegneria Informatica (LII)
===========================================

This directory contains the source code of the project developed for LII

Laboratorio di Ingegneria Informatica
Laurea Triennale in Ingegneria dell'Informazione
Universita' degli Studi di Padova, Italia


Copyright and license information can be found in the file LICENSE. 
Additional information can be found in the file NOTICE.


<h1> Installation </h1>

Create you own Link directory
  
  $ mkdir link
  $ cd link
  
Download the repository
  $ git clone https://github.com/davidetalon/link.git
  
Compile using cmake
  $ cd src
  $ cmake .
  $ make
  
Add Link to path
  $ PATH=<path to link>:$PATH
  
Discover Link using  
  $ link --help
