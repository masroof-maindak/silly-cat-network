import axios from "axios";
export const APIURL = "http://localhost:1234/content_receiver";

// timestamp for post ID
export const currentTimestampInSeconds = Math.floor(new Date().getTime() / 1000);

// parse string and delimit by ~ or :
export function convertStringToJson(inputString) {
  const keyValuePairs = inputString.split("~").filter(Boolean); // Remove empty strings

  const jsonObject = {};

  keyValuePairs.forEach((pair) => {
    const [key, value] = pair.replace("!", "_").split(":");
    jsonObject[key] = value || undefined; // Use undefined if the value is not provided
  });

  return jsonObject;
}

// transaction id
export function generateRandomSixDigitNumber() {
  const min = 100000000;
  const max = 999999999;
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

//fetches all posts if no data is passed
//else fetches the post with the unique key passed (i.e all posts of a user)
export const getAllPosts = async (dataelement) => {
    let dataString
    if(!dataelement){
        const response = await axios.post(APIURL, generateRandomSixDigitNumber()+"`filter`post``");
        dataString = response.data; // Assuming the response is a string
    }else{
        dataString = dataelement
    }
        const dataArray = dataString.split("~");
  const apiCallPromises = dataArray.map(async (element) => {
    const individualApiResponse = await axios.post(
      APIURL,
      generateRandomSixDigitNumber()+"`fetchVertexProperties`" + element + "`post`"
    );
    return `${individualApiResponse.data}uniquekey:${element}`;
  });
  const apiCallResponses = await Promise.all(apiCallPromises);
  console.log(apiCallResponses);
  const jsonResponseArray = apiCallResponses.map((responseString) => {
    const [desc, timestamp, uniquekey] = responseString
      .split("~")
      .map((item) => item.split(":")[1]);

    return { desc, timestamp, uniquekey };
  });
  return jsonResponseArray;
};

// fetches all posts of a user
export const getUserPosts = async (element) => {
    console.log(element);
  try {
    const individualApiResponse = await axios.post(
      APIURL,
      //req server for data
      generateRandomSixDigitNumber()+`fetchVertexProperties\`${element}\`post\``
    );

    const responseString = `${individualApiResponse.data}uniquekey:${element}`;

    //split the response string and get the required data
    const [desc, timestamp, uniquekey] = responseString
      .split("~")
      .map((item) => item.split(":")[1]);

    // create a json object of a post's data
    const userData = { desc, timestamp, uniquekey };

    console.log(userData);

    return userData;
  } catch (error) {
    console.error(error);
    throw error;
  }
};

  
