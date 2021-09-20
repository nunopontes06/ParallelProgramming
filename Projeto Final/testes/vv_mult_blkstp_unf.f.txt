c
c*******************************************************************
c
c		C-DAC Tech Workshop : hyPACK-2013
c                           October 15-18, 2013
c
c  Example 13		:vv_mult_blkstp_unf.f
c
c  Objective           : Vector_Vector Multiplication (Using blockstriped 
c                        uniform data Partitioning ) 
c
c  Input               : Read files vdata1.inp for Vector_A and vdata2.inp 
c                        for Vector_B 
c
c  Output              : Process with Rank 0 prints the Vector-Vector 
c                        Multiplication results 
c
c  Necessary Condition : Number of Processes should be
c                        less than or equal to 8
c
c   Created           : August-2013
c
c   E-mail            : hpcfte@cdac.in     
c
c*******************************************************************

         program main

         include 'mpif.h'
         integer DATA_SIZE
         real Epsilon

	 parameter (Epsilon = 1.0E-10, DATA_SIZE = 16)

  	 integer   Numprocs, MyRank
  	 integer   VectorSize, ScatterSize 
  	 integer   VectorSize_A, VectorSize_B 
  	 integer   index
  	 integer   Root
  	 real  	   Mybuffer_A(DATA_SIZE), Mybuffer_B(DATA_SIZE)  
	 real	   MyFinalVector, FinalAnswer
  	 real 	   Vector_A(DATA_SIZE), Vector_B(DATA_SIZE)


C   ........MPI Initialisation .......

   	 call MPI_INIT(ierror) 
   	 call MPI_COMM_RANK(MPI_COMM_WORLD, MyRank, ierror)
   	 call MPI_COMM_SIZE(MPI_COMM_WORLD, Numprocs, ierror)

	 Root = 0

   	 if(MyRank .eq. Root) then 
 		 open(unit=12, file = './data/vdata1.inp')
		 read(12,*) VectorSize_A
		 read(12,*) (Vector_A(i), i=1,VectorSize_A)

 		 open(unit=12, file = './data/vdata2.inp')
		 read(12,*) VectorSize_B
		 read(12,*) (Vector_B(i), i=1,VectorSize_B)
	 endif

C     .......Allocate memory and read data for vector A .....

      call MPI_BCAST(VectorSize_A, 1, MPI_INTEGER, Root, 
     $		 MPI_COMM_WORLD, ierror)

      call MPI_BCAST(VectorSize_B, 1, MPI_INTEGER, Root, 
     $		 MPI_COMM_WORLD, ierror)

       if(VectorSize_A .ne. VectorSize_B) then
	  if(MyRank .eq. Root) then
          print *,"Two Vectors can not be multiplied - exit"
	  endif
	  goto 100
       else
        VectorSize = VectorSize_A
	endif
	 
       if(VectorSize .lt. Numprocs) then
	 if(MyRank .eq. Root) then
         print *,"VectorSize should be more than No of Processors"
	 endif
	 goto 100
       endif


        if(mod(VectorSize, Numprocs) .ne. 0) then
	 if(MyRank .eq. Root) then
	 print*,"VectorSize Can not be Striped Evenly ..... "
	 endif
	 goto 100
	endif

C    ...... Scatter vector A and B .......

	ScatterSize = VectorSize / Numprocs
	call MPI_SCATTER(Vector_A,ScatterSize,MPI_REAL,Mybuffer_A, 
     $		 ScatterSize,MPI_REAL,Root,MPI_COMM_WORLD, ierror)
			 
	call MPI_SCATTER( Vector_B,ScatterSize, MPI_REAL,Mybuffer_B, 
     $		 ScatterSize,MPI_REAL,Root,MPI_COMM_WORLD,ierror)

C	  ....... Calculate partial sum .......  

	 MyFinalVector = 0.0
	 do index = 1, ScatterSize
	  MyFinalVector = MyFinalVector +
     $     	          Mybuffer_A(index)*Mybuffer_B(index)
	 enddo

C	  .... Collective computation : Final answer on process 0 
   	 call MPI_REDUCE(MyFinalVector, FinalAnswer, 1, MPI_REAL, 
     $		 MPI_SUM, Root, MPI_COMM_WORLD, ierror)

	 if(MyRank .eq. 0) then
		print*,'FinalAnswer', FinalAnswer
	 endif

 100	 call MPI_FINALIZE(ierror)
	 stop
	 end



