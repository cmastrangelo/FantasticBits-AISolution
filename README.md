# FantasticBits-AISolution

## Description
My solution to the "Fantastic Bits" challenge from CodinGame, a competitive programming contest. Developed in C++, this project showcases an AI-driven approach to game strategy, using advanced algorithms for geometry calculation, simulation, and decision-making.

## Ranking
Ranked 200th out of 2399 participants.

## Challange Explanation
FantasticBits consists of developing an algorithm to control two agents who are playing versus another 2 enemy agents in a 2D map. The goal is to score points by moving towards a ball, throwing spells on the ball and attempting to score on the opposite side.

## Solution Strategy
This solution consists of using geometry to calculate distances and angles in which to move and cast spells. I created a small simulation of the movements in order to predict enemy movement and when the ball will be at a specific time based on the information available.

Also using geometry and the ball speed I was able to calculate the path that the ball is moving to and where to aim when throwing. This allows the agents to make throws at the correct direction, allows me to block shots towards my goal and allows me to cancel potential throws when the enemy is blocking their goal, forcing agent to look for another solution (passing the ball to teamate potentially).

## Key Technical Highlights
Decision tree was particularly effective for this specific challange. I believe programming the agent's consideration for "passing" the ball instead of shooting at an enemy agent offered this solution a great advantage, beating many solutions that overlooked the power of passing the ball. Attempting to leverage instead only strong and optimized montecarlo tree-search algorithms with no hardcoded logic into it seemed to fall behind in the rankings performing in unpredictable ways.

## Challanges and Solutions
During the final phase of the challange, a new spell for dragging the ball was introduced and programmers were able to develop a strong strategy of goalkeeper with this spell. This solution dropped around 100 ranks during that time. I attempted to code a hotfix in order to incorporate this goalkeeper behavior, unfortunatelly running out of time without finding a proper consistent solution to the problem. I instead deactivated that behavior and was able to get 200th rank with the previous solution. There are however, other potential hardcoded logic that could've improved performance if I had gotten to in time, such as blocking passing either with agents or spells.

## Conclusions
I believe implementing a proper goalkeeper solution should acquire another 50 ranks. In order to compete in the top 100 a monte carlo tree search or a genetic algorithm would probably have to be incorporated within the current solution. All this considered I was proud of the performance of the code in the challenge. I believe the greatest strength was developing a strong algorithm for passing the ball between agents and predicting the passing ahead of friendly agent. This seemed to outperform a lot of the more elaborate decision trees.
