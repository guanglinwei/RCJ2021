# Gallery example

- Make the gallery with a code block
```gallery
```

  
- Add the options in JSON format
```gallery
{"type": ..., "width": ..., "height": ...}
```
Options properties:
- If "type" is "table", display the al images at once in a table. Must set width and height properties.
- If no properties are provided, uses a carousel (rotating image display). No other properties needed.


  
- Add images
```gallery
{"type": "table", "width": 3, "height": 3}
imageName1.jpg, 480, 360, The first image
imageName2.png, 1920, 1080, Second image
```
In each line, provide the following:
- the name of the image
- width  *(optional)*
- height  *(optional)*
- alt text/label. You can include quotes around the alt text if you need commas. (Ex: "This, text, has commas")  *(optional)*