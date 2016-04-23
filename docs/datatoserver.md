According to the problem statement, the data we want to collect is as follows.
 - Species
 - Length/breadth
 - position/datetime
 - User metadata

Addressing each of these individually, the types for each are as follows.

**species** is a string as this will allow for maximum flexibility.

**length** is an integer in mm as is **breadth**.

**position** is an object consisting of two doubles **longitude** and **latitude**.

**datetime** can be a string of format "yyyy/mm/dd:HH:MM:SS".

**user** will be a 128-bit GUID.

In JSON, this will look somewhat like the following:

```javascript
{
  species: "Fish",
  length: 154,
  breadth: 32,
  position: {
    lat: 53.4242453,
    long: 40.3232453
  },
  datetime: "1900/01/01:00:00:00",
  user: "96965dcd-1d6e-4758-8498-0a042c0134af"
}
