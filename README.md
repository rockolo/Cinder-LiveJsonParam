# Cinder-LiveJsonParam
A basic-live-cinder-json-saved-parameters system using _Simon Geilfus'_ Watchdog as a submodule.

```
live::Param<float> mRadius;
[...]
JsonBag::add( &mRadius, "radius" );
JsonBag::add( &mRadius, "radius",  []() { app::console() << "Updated radius!" << endl; } );
```
In `assets/`, the parameter values are stored as follows in the _live_params.json_ file.

```
{
   "params" : {
      "radius" : "0",
   }
}
```

Editing and saving that file will update the value in real-time.
