# QORPOTestJulian

Note: This is a recreation of the test I took to enter QORPO World on 11/27/2023.

Final result: Success

	Test Specification:

		1) Develop a character with basic movements (forward/backward, strafe, jump, crouch, sprint).
		2) Create a simple "raycasting" weapon that can be assigned to the character.
		3) Create the following "interactable" items:
			- Weapon pickup (picking up a second weapon or dropping a weapon is not required).
			- Health pickups.
			- Interactable doors.
			- Explosion barrels (causing damage to nearby players and making an impact force field).
		4) Implement reloading functionality for the weapon:
			- The weapon has a limited number of bullets in each magazine.
			- Generate magazine pickups on the ground, which, upon overlap, are taken and stored for the 
			character's use (during reloading).
		5) Develop a simple survival game mode that tallies the player's score for killed enemies. After the 
		character's death, report the survived time and score to the user via the UI.
		6) Create a straightforward UI displaying users' current statistics (health, bullets left, magazine, 
		survived time, and score).
		7) Ensure the code is multiplayer-ready, allowing correct functionality when playing with multiple 
		clients. Players should be able to engage in the survival game.
		8) Implement a projectile weapon.

	Further description:

		- Emphasize the use of C++ over blueprints, as this test primarily assesses C++ knowledge, OOP practices, 
		and clean code.
		- The candidate test focuses on code structuring and working processes.
		- Commit your progress regularly.
		- Some of the last testing tasks are intended as a bonus.