import("plplotdll.dll")

image = plimload("C:\Users\jyv\Pictures\2018-04\IMG_0054.JPG",x,y,n,3)

print(x,' ',y,' ',n)

#plimwrite("C:\Users\jyv\Pictures\2018-04\IMG_0054_2.png",x,y,3,image,0,"png")

image_small = plimresize(image,x,y,x*3,x / 10,y / 10,x/10*3,3)
plimwrite("C:\Users\jyv\Pictures\2018-04\SML_0054.jpg",x / 10, y / 10,3,image_small,100,"jpg")

plimfree(image)
plimfree(image_small)

