# bigbob.inc
# model for the robot "Bigbob"
# Author: Jennifer Owen
# Date: 16/04/2010

#Bigbob's sonars
define bigbobs_sonars ranger
(
      # number of sonars
      scount 4

      # define the pose of each transducer [xpos ypos heading]
      spose[0] [ 0.125 0 0]	#fr front tooth
      spose[1] [ 0 .125 90]	#fr left tooth
      spose[2] [ -.125 0 180]		# back corner
      spose[3] [ 0 -0.125 -90]	# right corner
		
      # define the field of view of each transducer 
      #[range_min range_max view_angle]
      sview [0 2.0 10]

      # define the size of each transducer
      # [xsize ysize] in meters
      ssize [0.001 0.005]
)



# bigbob's body
define bigbob position
(
      # actual size
      size [.25 .25 1]

      # Bigbob's centre of rotation is offset from its centre of area
      origin [0 0 0 0]

      # the shape of Bigbob
 
	block
	(
		points 4
		point[3] [0 0]
		point[2] [0 .25]
		point[1] [.25 .25]
		point[0] [.25 0]
		z [0 1]
	)
  

	
      # differential steering model
      drive "diff"

	
	# sensors attached to bigbob
	bigbobs_sonars()  
	#ranger_return 0             	# reflects sonar beams
) 



