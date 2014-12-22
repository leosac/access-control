@namespace Leosac::Hardware
@brief Holds facade classes to hardware device implementation.

Classes in this namespace are classes that hide message-passing detail
to their caller.

Since devices support is implemented through module, we need to send message
to communicate with the device. Those classes provide a nice abstraction
to do so with traditional method call instead of manually sending message.

@note It is recommended to use theses classes to interact with a device.
