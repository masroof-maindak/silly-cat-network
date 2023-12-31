import React from "react";

const PostCard = ({description, username, timestamp}) => {
  return (
    <div>
        <div className="mb-3">
          <div className="card p-1" style={{borderRadius: "10px", background: "#3a3124"}}>
          <div style={{height: "250px", borderRadius: "10px 10px 0px 0px"}} className="card-body">
            <p style={{}}>{description}</p>
          </div>
            <div style={{ borderRadius: "0px 0px 10px 10px", padding: "0px 25px"}} className="d-flex justify-content-between">
              <p style={{}}>Posted on: {timestamp}</p>
              <p style={{}}>Posted by: {username}</p>
            </div>
        </div>
      </div>
    </div>
  );
};

export default PostCard;
