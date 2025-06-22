// Hash function from https://www.shadertoy.com/view/4djSRW
uint hash(uint x) {
    x += (x << 10u);
    x ^= (x >> 6u);
    x += (x << 3u);
    x ^= (x >> 11u);
    x += (x << 15u);
    return x;
}

// Pseudo-random number generator using XOR shift
uint rng_xorshift(inout uint state) {
    state ^= (state << 13);
    state ^= (state >> 17);
    state ^= (state << 5);
    return state;
}

// Returns a floating point value between 0 and 1
float rng_next(inout uint state) {
    return float(rng_xorshift(state)) / float(0xFFFFFFFFu);
}