import React from "react";

const PostCard = ({ description, username, timestamp }) => {
  return (
    <div>
      <div className="mb-5 mt-5" style={{ width: "80%", margin: "auto"}}>

        <div className="card p-1 m-5" style={{ borderRadius: "14px", background: "#4d3f32" }}>

          <div style={{ height: "200px", borderRadius: "10px 10px 0px 0px" }} className="card-body">
            <p style={{ background: "#3A3124" }}>{description}</p>
          </div>

          <div style={{ borderRadius: "0px 0px 10px 10px", padding: "0px 25px", background: "#3a3124" }} className="d-flex justify-content-between">
            <p style={{ background: "#3A3124" }}>Posted on: {timestamp}</p>
            <p style={{ background: "#3A3124" }}>Posted by: {username}</p>
          </div>

        </div>

      </div>
    </div>
  );
};

export default PostCard;
