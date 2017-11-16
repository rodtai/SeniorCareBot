// Include Pixy libraries
#include <SPI.h>
#include <Pixy.h>


// Motor shield libraries
#include <Servo.h>
// Declare objects:
Pixy pixy;

Servo rightMotor;
Servo leftMotor;

void setup() {
  Serial.begin(9600);
  Serial.print("Starting...\n");
  
  pixy.init();
  rightMotor.attach(9);
  leftMotor.attach(10);
}

uint32_t lastBlockTime = 0;

void loop() {
  uint16_t blocks;
  blocks = pixy.getBlocks(); // read pixy

  // if there are blocks, track and follow them
  if (blocks)
  {
    int trackedBlock = TrackBlock(blocks);
    FollowBlock(trackedBlock);
    lastBlockTime = millis();
  }
  // stop if no blocks
  else if (millis() - lastBlockTime > 100)
  {
    rightMotor.writeMicroseconds(1500);
    leftMotor.writeMicroseconds(1500);
  }

  // ADD DELAY WHEN ADJUSTING SPEED
//   delay(1000);

}


int oldSignature;
// find biggest block
int TrackBlock(int blockCount)
{
  int trackedBlock = 0;
  long maxSize = 0;

  Serial.print("blocks = ");
  Serial.println(blockCount);

  for (int i = 0; i < blockCount; i++)
  {
    // if statement not needed
    if ((oldSignature == 0) || (pixy.blocks[i].signature == oldSignature))
    {
      long newSize = pixy.blocks[i].height * pixy.blocks[i].width;
      if (newSize > maxSize)
      {
        trackedBlock = i;
        maxSize = newSize;
      }
    }
  }

  oldSignature = pixy.blocks[trackedBlock].signature;
  return trackedBlock;
}

// follow the object, try to keep object in center and keep some distance
int32_t size = 400;
void FollowBlock(int trackedBlock)
{
  // average of last 8 sizes
  size += pixy.blocks[trackedBlock].width * pixy.blocks[trackedBlock].height;
  size -= size >> 3;
  

  // set speed based on distance (with a given size) CHANGE NUMBERS UNDER ARROWS AS NECESSARY
  //                            \/          \/  \/  \/ 0(stopped) 255(full speed)  
  int forwardSpeed = constrain((256 - (size/512)), 0 ,255);


  // differential (keeps object centered) 
  //                                \/                                                    \/
  int32_t differential = ((int32_t)159 - (int32_t)pixy.blocks[trackedBlock].x)/ (int32_t)2.2;

  // adjust with differential
  int leftSpeed = constrain(forwardSpeed - differential, 0, 255);
  int rightSpeed = constrain(forwardSpeed + differential, 0, 255);

  // set speeds
  rightMotor.writeMicroseconds(1500 - rightSpeed/2);
  leftMotor.writeMicroseconds(1500 - leftSpeed/2);

  // print to serial  - USE TO ADJUST SPEED
  Serial.print("differential: ");
  Serial.print(differential);
  Serial.println(" (positive is left)");
  Serial.print("forward speed: ");
  Serial.println(forwardSpeed);
  Serial.print("left speed: ");
  Serial.println(leftSpeed);
  Serial.print("right speed: ");
  Serial.println(rightSpeed);
  Serial.print("size: ");
  Serial.println(size);
 
}

