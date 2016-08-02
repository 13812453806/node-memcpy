{
  "targets": [
    {
      "target_name": "memcpy",
      "sources": [ "src/memcpy.cc", "src/NativeExtension.cc" ],
      "include_dirs" : [
          "<!(node -e \"require('nan')\")",
          "<!(node -e \"require('node-arraybuffer')\")"
      ]
    }
  ]
}
