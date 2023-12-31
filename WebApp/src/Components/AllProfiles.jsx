import { Button, Pagination } from 'antd';
import axios from 'axios';
import React, { useEffect, useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { APIURL, generateRandomSixDigitNumber } from '../APIS/Constant';

const AllProfiles = () => {
  const navigate = useNavigate();
  const [profiles, setProfiles] = useState([]);
  const [currentPage, setCurrentPage] = useState(1);
  const profilesPerPage = 6;

  useEffect(() => {
    axios
      // request to get all vertices of type cat
      .post(APIURL, generateRandomSixDigitNumber() + "`filter`cat``")
      .then((res) => {
        const newData = res.data.split("~").map((item) => ({
          name: item,
        }));
        setProfiles(newData);
      })
      .catch((err) => alert('Something Went Wrong'));
  }, []);

  const indexOfLastProfile = currentPage * profilesPerPage;
  const indexOfFirstProfile = indexOfLastProfile - profilesPerPage;
  const currentProfiles = profiles.slice(indexOfFirstProfile, indexOfLastProfile);

  const handlePageChange = (page) => {
    setCurrentPage(page);
  };

  return (
    <div>
      <div className='row m-4'>
        <div className='col-8'></div>
        <div className="col-1">
          <Button type="primary" style={{ background: "#db930d" }} onClick={() => navigate('/posts')}>
            <span style={{ background: "#db930d", color: "#242015" }}>
              Show All Posts
            </span>
          </Button>
        </div>
      </div>
      <div className='row'>
        {currentProfiles.map((item, index) => (
          <div key={index} className='col-4 d-flex justify-content-between' style={{}}>

            <div className='card mb-3 p-1' style={{ width: "300px", background: "#3a3124" }}>

              <div className='card-body'>
                <h5 style={{ color: "#ebe0bb", background: "#3a3124" }}>{item.name}</h5>
                
                <Button type='primary' onClick={() => {
                  navigate('/userposts', { state: { item } })
                }}
                  style={{ background: "#db930d" }}
                >
                  <span style={{ background: "#db930d", color: "#242015" }}>Open Posts</span>
                </Button>
              
              </div>
            </div>
          </div>
        ))}
      </div>
      <div className="pagination-container">
        <Pagination
          current={currentPage}
          pageSize={profilesPerPage}
          total={profiles.length}
          onChange={handlePageChange}
        />
      </div>
    </div>
  );
};

export default AllProfiles;
