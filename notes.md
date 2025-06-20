## To-do 
- [ ] Need to add Root object and propreties to the TDMS file.
- [ ] Need to write Root, MetaData and ChannelData on first write. 
- [ ] Ensure that the correct tocMask is being used for subsequent writes. 
- [ ] Ensure that files are closed off correctly


## 06/10/2025
- I want to switch to the header generation stuff from structs to classes so I can do some oop to simplify the data generation portion.
- Now ... is that pythonic? idk...
- I should name space my TDMS stuff to keep it safe

## 06/18/2025

- I want to better under stand how th NI TDMS File structure works

[TMDS File Format Internal Structure](https://www.ni.com/en/support/documentation/supplemental/07/tdms-file-format-internal-structure.html?srsltid=AfmBOookSFum294uK7L5jSDPN_VB-4Rak8-xAsLs35Vfw_VPa9-sLOWx)


### Overview
- The simple example that is given shows the following


| Obj Name | Object | Path |
| -------- | ------ | ---- |
| -- | File | `/` |
| Measured Data | Group | `/'Measured Data'` |
| Ampliture Sweep | Channel | `/'Measured Data'/'Amplitude Sweep'` |
| Dr. T's Events | Group | `/'Dr. T's Events'` |
| Time | Channel | `/'Dr. T's Events'/'Time'` |


- TDMS files must contain file objects, file objects must contain group objects for each group used in a channel.
- Files can have an arbitrary number of groups, groups may have no channels.

- TDMS Objects (Files, Groups, Channels) can have an infinite amount of properties, these properties are defined :
    - String : Name
    - Type ID : ?
    - Value : of type (Type ID)
- Typical data types, floats and fixed up to 64 bytes in size
- No arrays, ie properties can only have a single value 

### Binary Layout
- Every TDMS file has meta data and raw data. 
    - Meta data: the descriptive data stored in objects or properties
    - Raw data: the data arrays associated with the channels 
- TDMS files use a raw data index, which is partially comprised of the information on composition.

### TDMS Segment Layout
- Data is written in segments to TDMS files. When data is appended a new segment is created.
- Segments consist of the following
    - **Lead In** : Basic info, and mostly standarized
    - **Meta Data**: Property data for this segment, and information required to locate raw data associated with a channel.
    - **Raw Data**: A contiguous block of raw data associated with any objects indluded in the segment. 

- **Strings** are encoded using UTF-8, proceeded by a `uint32_t` that designates the length of the string in bytes.
- **Timestamps** are encoded as:
    - `int64_t`: seconds since epoch
    - `uint64_t`: positive fractions of $2^{-64}$ of a second
- **Bool** are a single byte 

#### Lead In 
- Contains the info required to validate a segment 
- Example:

| Offset | Hex | Desc. |
| ------ | ------ | ----- |
| 4 | `54 44 53 6D` | "TDSm" tag |
| 8 | `0E 00 00 00` | ToC mask `0x1110` (seg. contains: obj list, meta data, raw data) |
| 12 | `69 12 00 00` | Version Number (4713) |
| | `E6 00 00 00` | Next segment offset |
| 20| `00 00 00 00` | - Value (230)|
| | `DE 00 00 00` | Raw Data offsets |
| 28 | `00 00 00 00` | - Value (222) |

- **Segment Offset**: uses 8 bytes to describe the overall length of the segemtn minus the the lead in.
    - For the final segment of the file all bytes can be `FF`
- **Raw Data Offset**: Describes the overall length of the meta data. Meaning the raw data size is the **Segment Offset - Raw Data Offset**
    - If there is no meta data this value can be zero

##### List of ToC:

| Flag | Value | Desc. |
| ---- | ----- | ----- |
| `kTocMetaData` | `0000 0001` | Meta Data |
| `kTocNewObjList` | `0000 0010` | New Obj List (new channels) |
| `kTocRawData` | `0000 0100` | Raw Data |
| `kTocInterleavedData` | `0001 0000` | Interleaced Data (otherwise contiguous)|
| `kTocBigEdian` | `0010 0000` | All values excluding ToC are Big Edian |
| `kTocDAQmxRawData` | `0100 0000` | Raq DAQmx data |

- Example: segment contains meta data, new object list and raw data 
    - ToC would be: `0000 0111`

#### Meta Data 
- Consists of a 3 level hierachy of data Objects
    - File 
        - Groups 
            - Channels 
- Each object can have any number of properties. 
- The binary rep of meta data is as follows:
    - The number of onew objects in this segment (`uint32_t`)
    - The binary representation of these objects 

- The binary layout of an object can have the following components and order 
    - **Object path**  (`string`)
    - **Raw Data Index**
        - **Length** of the raw data index (`uint32_t`)
        - **Data type** (`tdsDataType` as `uint32_t`)
        - **Array Dimension** only for TDMS v2.0 (`uint32_t`)
        - **Number of Values** (`uint64_t`)
        - **Total Size in Bytes** (`uint64_t`) only stored for string
        - If there is no raw data store (`0xFFFFFFFF`)
        - If there is duplicate data from the last segment write (`0x00000000`)
    - **Number of Properties** (`uint32_t`)
    - **Properties**
        - **Name** (`String`)
        - **Data Type** (`tdsDataType` as `uint32_t`)
        - **Value** (Data Type)

## 06/20/2025
- Worked on some binary conversion stuff for properties, meta data like channels 
- Tried to OOP a little bit 
- Templates are interesting 


