import React from "react";
import { Route, Routes } from "react-router-dom";
import Posts from "./Pages/Posts";
import AllProfiles from "./Components/AllProfiles";
import SingleUserPosts from "./Pages/SingleUserPosts";

const Routs = () => {
  return (
    <div className="m-10">
      <Routes>
        <Route path="posts" element={<Posts />} />
        <Route path="friends" element={<AllProfiles />} />
        <Route path="userposts" element={<SingleUserPosts />} />
      </Routes>
    </div>
  );
};

export default Routs;
