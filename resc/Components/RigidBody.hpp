struct Rigidbody {
    bool isGravity;
    bool isKinematic;
    float mass;
    float gravity;
    float velocity[3];
    float acceleration[3];
    float drag;
    float angularDrag;
};