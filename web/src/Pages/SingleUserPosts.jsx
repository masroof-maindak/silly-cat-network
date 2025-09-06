import React, { useEffect, useState } from "react";
import { Button, Pagination } from "antd";
import PostCard from "../Components/PostCard";
import axios from "axios";
import NewPost from "../Components/NewPost";
import { useLocation, useNavigate } from "react-router-dom";
import { APIURL, generateRandomSixDigitNumber, getAllPosts } from "../APIS/Constant";

const SingleUserPosts = () => {
  const [data, setData] = useState([]);
  const [fetchUser, setFetchUser] = useState("");
  const [currentPage, setCurrentPage] = useState(1);
  const [itemsPerPage] = useState(5);
  const [visible, onClose] = useState(false);
  const navigate = useNavigate();
  const location = useLocation();
  const profilesData = location.state?.item || [];

  useEffect(() => {
    axios
      .post(
        APIURL,
        //req server for all posts of a user
        generateRandomSixDigitNumber() + "`relationalQuery`" + profilesData.name + "`cat`post`posts``"
      )
      .then((res) => {
        const user = res.data.replace(/~$/, "");
        setFetchUser(user);
      })
      .catch((err) => {
        alert("Something Went Wrong");
      });
  }, [APIURL, profilesData.name]);
  console.log(fetchUser);
  useEffect(() => {
    if (fetchUser != "") {
      getAllPosts(fetchUser).then((res) => {
        const newData = res.map((item) => {
          return {
            username: item.uniquekey,
            description: item.desc,
            timestamp: item.timestamp,
          };
        });
        setData(newData);
      });
    }
  }, [fetchUser]);

  const indexOfLastItem = currentPage * itemsPerPage;
  const indexOfFirstItem = indexOfLastItem - itemsPerPage;
  const currentItems = data.slice(indexOfFirstItem, indexOfLastItem);

  const handleChangePage = (page) => {
    setCurrentPage(page);
  };

  return (
    <div className="container">
      <NewPost visible={visible} onClose={onClose} />
      <div className="row m-4">
        <div className="col-7"></div>
        <div className="col-1">
          <Button
            type="primary"
            style={{ backgroundColor: "#db930d" }}
            onClick={() => {
              onClose(true);
            }}
          >
            <span style={{ background: "#db930d", color: "#242015" }}>Create Post</span>
          </Button>
        </div>
        <div className="col-1"></div>
        <div className="col-1">
          <Button
            type="primary"
            style={{ backgroundColor: "#db930d" }}
            onClick={() => {
              navigate("/friends");
            }}
          >
            <span style={{ background: "#db930d", color: "#242015" }}>Show Users</span>
          </Button>
        </div>
        <div className="col-1"></div>
        <div className="col-1">
          <Button
            type="primary"
            style={{ backgroundColor: "#db930d" }}
            onClick={() => {
              navigate("/posts");
            }}
          >
            <span style={{ background: "#db930d", color: "#242015" }}>Show All Posts</span>
          </Button>
        </div>
      </div>
      <div className="m-4">
        {currentItems.map((item, index) => (
          <PostCard
            key={index}
            description={item.description}
            username={item.username}
            timestamp={item.timestamp}
          />
        ))}
      </div>
      <Pagination
        current={currentPage}
        total={data.length}
        pageSize={itemsPerPage}
        onChange={handleChangePage}
        showSizeChanger={false}
        style={{ textAlign: "center", marginBottom: "20px"}}
      />
    </div>
  );
};

export default SingleUserPosts;
