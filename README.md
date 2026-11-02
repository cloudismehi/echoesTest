# Echoes test

This project is all about modeling room reverberation using delay lines. The current idea for the project is to use raytracing to map out delays and attenuations and store these numbers to be used with a simple delay line class. 

## Current progress 

I have made a crude ray-tracing model that treats sound as a ball bouncing perfectly around the room with no attenuation. Currently the main function in the raytracing function allows you to give it a number of rays on the $xy\text{-plane}$ and a number of rays on the perpendicular $z\text{-plane}$ as well as the maximum number of collisions per ray and it returns back a list of delay times and distances carried by the particle which will be needed in the delay line to model the reverberation in the room.

**the next big implementation** I need is to implement the attenuation on the sound due to both collisions with the wall as well as due to the travel through the space of the room. The current implementation has the problem of blowing up for large reverberation rooms or reverberators, I think the attenuation should help aliviate these effects. 