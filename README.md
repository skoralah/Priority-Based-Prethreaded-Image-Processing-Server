# Priority-Based-Prethreaded-Image-Processing-Server

1] An image processing server is being designed that interacts with the client and server which includes a main thread and a pool of worker threads.


2] The main thread is given higher priority than worker threads.


3] The client inputs the user-input color image to the server and server applies image processing operations on the image input by the client and sends back to the client.


4] The main thread repeatedly accepts connection requests from client and places the resulting connected descriptors in a bounded buffer. Each worker thread repeatedly removes a descriptor from the buffer, services the client, and then waits for the next descriptor.


5] OpenCV library is used as a platform for image processing operations.

