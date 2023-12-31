import React, { useEffect, useState } from "react";
import { Button, Pagination } from "antd";
import PostCard from "../Components/PostCard";
import NewPost from "../Components/NewPost";
import { useNavigate } from "react-router-dom";
import { getAllPosts } from "../APIS/Constant";

const Posts = () => {
  const [data, setData] = useState([]);
  const [currentPage, setCurrentPage] = useState(1);
  const [itemsPerPage] = useState(5);
  const [visible, onClose] = useState(false);
  const navigate = useNavigate();

  useEffect(() => {
    getAllPosts()
      .then((res) => {
        const newData = res.map((item) => {
          return {
            username: item.uniquekey,
            description: item.desc,
            timestamp: item.timestamp,
          };
        });
        setData(newData);
      })
      .catch((err) => {
        alert("Something Went Wrong");
      });
  }, []);

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
        <div className="col-9"></div>
        <div className="col-1">
          <Button
            type="primary"
            style={{background: "#db930d"}}
            onClick={() => {
              onClose(true);
            }}
          >
            <span style={{background: "#db930d", color: "#242015"}}>
            Add New Post
            </span>
          </Button>
        </div>
        <div className="col-1"></div>
        <div className="col-1">
          <Button
            type="primary"
            onClick={() => {
              navigate("/friends");
            }}
            style={{background: "#db930d"}}
          >
          <span
            style={{background: "#db930d", color: "#242015"}}
          >
            Show Users
          </span> 
          
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
        style={{ textAlign: "center" }}
      />
    </div>
  );
};

export default Posts;
